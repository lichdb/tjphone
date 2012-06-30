/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006  Simon MORLAT (simon.morlat@linphone.org)

enable linphone to using ADI's libbfgdots g729 lib. 

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/


/**
 * bitstream format of libbfgdots - g729a/g729ab lib
 * 
 * using 0x6b21 as SYNC word for each frame
 *   [SYNCWORD][BIT_NUM][bits]
 * 
 * Packed mode:
 * 
 * - g729a:
 *   [0x6b21][0x0050][80-bit]
 *
 * - g729ab:
 *   three kinds of frames
 *   -- BIT_NUM = 0x0050
 *   -- BIT_NUM = 0x0010
 *   -- BIT_NUM = 0x0000
 *
 * Unpacked mode:
 *   each bit is encoded using a 16-bit word:
 *   - 0x007F for 0
 *   - 0x0081 for 1
 */

#include "mediastreamer2/msfilter.h"

#include <va_g729a.h>

#define FMT_PACKED 0
#define FMT_UNPACKED 1

#define _ADSP_G729_H_LEN 2
#define _PCK_BITSTREAM_LEN 80
#define _PCK_BITSTREAM_LEN_BYTES (_PCK_BITSTREAM_LEN/8)
#define _PCK_LEN_MS 10
#define _SAMPLES (_PCK_LEN_MS * 8)
#define _PCK_SIZE (_SAMPLES * sizeof(short))


typedef struct EncState{
	uint32_t ts;
	int enc_vad;
	int pck_len;
	MSBufferizer *bufferizer; /* a queue */
} EncState;


static void enc_init(MSFilter *f){
	EncState *s=ms_new(EncState, 1);
	memset(s, 0 , sizeof(*s));
	s->enc_vad = 0;
	s->ts=0;
	s->pck_len = 20;
	s->bufferizer=ms_bufferizer_new();
	f->data=s;
	va_g729a_init_encoder();
}

static void enc_uninit(MSFilter *f){
	EncState *s=(EncState*)f->data;
	ms_bufferizer_destroy(s->bufferizer);
	ms_free(s);
}

static void enc_process(MSFilter *f){
	EncState *s=(EncState*)f->data;
	mblk_t *im;
	const int n_pck = s->pck_len/_PCK_LEN_MS;
	int pck;
	int i,byte,bit;
	short ibuf[160];
	short obuf[_ADSP_G729_H_LEN + _PCK_BITSTREAM_LEN];
	int obuf_len_byte = n_pck * _PCK_BITSTREAM_LEN_BYTES;

	while((im=ms_queue_get(f->inputs[0]))!=NULL){
		ms_bufferizer_put(s->bufferizer,im);
	}
	while(ms_bufferizer_read(s->bufferizer,(uint8_t*)ibuf,sizeof(ibuf))==sizeof(ibuf)) {
		mblk_t *om=allocb(obuf_len_byte,0); 
		memset(om->b_wptr, 0 , obuf_len_byte);
		for (pck=0;pck<n_pck;++pck){
			va_g729a_encoder(ibuf + (pck * _SAMPLES), om->b_wptr);
			om->b_wptr+=_PCK_BITSTREAM_LEN_BYTES;
		}
		mblk_set_timestamp_info(om,s->ts);
		ms_queue_put(f->outputs[0],om); /* one frame per packet */
		s->ts+=sizeof(ibuf)/sizeof(short);
	}
}

#ifdef _MSC_VER

MSFilterDesc ms_g729_enc_desc={
	MS_G729_ENC_ID,
	"MSG729Enc",
	"The G729a(b) codec",
	MS_FILTER_ENCODER,
	"g729",
	1,
	1,
	enc_init,
	NULL,
	enc_process,
	NULL,
	enc_uninit,
	NULL
};

#else

MSFilterDesc ms_g729_enc_desc={
	.id=MS_G729_ENC_ID,
	.name="MSG729Enc",
	.text="The G729a(b) codec",
	.category=MS_FILTER_ENCODER,
	.enc_fmt="g729",
	.ninputs=1,
	.noutputs=1,
	.init=enc_init,
	.process=enc_process,
	.uninit=enc_uninit,
};

#endif

static void dec_init(MSFilter *f){
	va_g729a_init_decoder();
	f->data= NULL;
}


static void dec_process(MSFilter *f){
	mblk_t *im;
	mblk_t *om;
	int i,byte,bit;
	short _pck[_ADSP_G729_H_LEN + _PCK_BITSTREAM_LEN];
	while((im=ms_queue_get(f->inputs[0]))!=NULL){
		while (im->b_wptr - im->b_rptr >= _PCK_BITSTREAM_LEN_BYTES){
			om=allocb(_PCK_SIZE,0); /* 80 * 2 */
			va_g729a_decoder(im->b_rptr,(short *) om->b_wptr, 0);
			om->b_wptr += _PCK_SIZE;
			ms_queue_put(f->outputs[0],om);
			im->b_rptr += 10;
		}
		freemsg(im);
	}
}

#ifdef _MSC_VER

MSFilterDesc ms_g729_dec_desc={
	MS_G729_DEC_ID,
	"MSG729Dec",
	"The G729 codec",
	MS_FILTER_DECODER,
	"g729",
	1,
	1,
	dec_init,
	NULL,
	dec_process,
	NULL,
	NULL,
	NULL
};

#else

MSFilterDesc ms_g729_dec_desc={
	.id=MS_G729_DEC_ID,
	.name="MSG729Dec",
	.text="The G729 codec",
	.category=MS_FILTER_DECODER,
	.enc_fmt="g729",
	.ninputs=1,
	.noutputs=1,
	.init=dec_init,
	.process=dec_process,
	.uninit=NULL
};

#endif

MS_FILTER_DESC_EXPORT(ms_g729_dec_desc)
MS_FILTER_DESC_EXPORT(ms_g729_enc_desc)
