
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

#define IPPCORE_NO_SSE
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mediastreamer2/msfilter.h"

#include <ipp_w7.h>
#include <ippcore.h>
#include <ipps.h>
#include <ippsc.h>
#include "g729fpapi.h"
#include "owng729fp.h"
#include "usc_cdcs.h"
#include "usc.h"

#define FMT_PACKED 0
#define FMT_UNPACKED 1

#define _ADSP_G729_H_LEN 2
#define _PCK_BITSTREAM_LEN 80
#define _PCK_BITSTREAM_LEN_BYTES (_PCK_BITSTREAM_LEN/8)
#define _PCK_LEN_MS 10
#define _SAMPLES (_PCK_LEN_MS * 8)
#define _PCK_SIZE (_SAMPLES * sizeof(short))

struct translator_pvt {
	USC_CodecInfo pInfo;
	USC_Handle codec;	/* the encoder or decoder handle */
	USC_MemBank* pBanks;
	USC_PCMStream pcmStream;	/* Signed linear data */
	USC_Bitstream bitStream;	/* G.729 bits */

	int nbanks;
	int maxbitsize;
  	//int inFrameSize;
   	//int outFrameSize;

	short pcm_buf[160];
	unsigned char bitstream_buf[20];

	int tail;
};

#define g729_coder_pvt translator_pvt


static USC_Fxns *USC_CODEC_Fxns = NULL;
static struct g729_coder_pvt *enc_pvt = NULL;
static struct g729_coder_pvt *dec_pvt = NULL;


typedef struct EncState{
	uint32_t ts;
	int enc_vad;
	int pck_len;
	MSBufferizer *bufferizer; /* a queue */
} EncState;


static void enc_init(MSFilter *f){
	int i;
	USC_Status result;
	EncState *s=ms_new(EncState, 1);
	ippStaticInit();
	if(!USC_CODEC_Fxns)
		USC_CODEC_Fxns = &USC_G729AFP_Fxns;
	memset(s, 0 , sizeof(*s));
	s->enc_vad = 0;
	s->ts=0;
	s->pck_len = 20;
	s->bufferizer=ms_bufferizer_new();
	f->data=s;

	enc_pvt = malloc(sizeof(struct g729_coder_pvt));
	memset((void *)enc_pvt, 0, sizeof(struct g729_coder_pvt));
	if(enc_pvt) {
		result = USC_CODEC_Fxns->std.GetInfo((USC_Handle)NULL, &enc_pvt->pInfo);
		enc_pvt->pInfo.params.modes.truncate = 1;
  		enc_pvt->pInfo.params.direction = 0;
  		enc_pvt->pInfo.params.modes.vad = 0;
		result = USC_CODEC_Fxns->std.NumAlloc((const void *)&(enc_pvt->pInfo.params), &(enc_pvt->nbanks));
		enc_pvt->pBanks = NULL;
		enc_pvt->pBanks = (USC_MemBank*)ippsMalloc_8u(sizeof(USC_MemBank)*(enc_pvt->nbanks));
		if(!enc_pvt->pBanks) {
			ms_warning("enc_init malloc failed allocating %d bytes", sizeof(USC_MemBank)*(enc_pvt->nbanks));
			return;
		}
		memset((void *)enc_pvt->pBanks, 0, sizeof(USC_MemBank)*(enc_pvt->nbanks));
		result = USC_CODEC_Fxns->std.MemAlloc((const void *)&(enc_pvt->pInfo.params), enc_pvt->pBanks);

		for(i=0; i<enc_pvt->nbanks;i++) {
			if(enc_pvt->pBanks->nbytes > 0){
				if(!(enc_pvt->pBanks[i].pMem = ippsMalloc_8u(enc_pvt->pBanks->nbytes))) {
					ms_warning("enc_init malloc failed allocating %d bytes", enc_pvt->pBanks->nbytes);
					return;
				}else{
					memset((void *)enc_pvt->pBanks[i].pMem, 0, enc_pvt->pBanks->nbytes);	
				}
			}else{
				ms_warning("enc_init enc_pvt->pBanks->nbytes %d\n", enc_pvt->pBanks->nbytes);
				return;
			}
		}

		//enc_pvt->inFrameSize = getInFrameSize();
		//enc_pvt->outFrameSize = getOutFrameSize();

		/* enc_pvt->bitstream_buf = ippsMalloc_8s(enc_pvt->pInfo.maxbitsize); */

		enc_pvt->pcmStream.bitrate = enc_pvt->pInfo.params.modes.bitrate;
  		enc_pvt->maxbitsize = enc_pvt->pInfo.maxbitsize;
  		result = USC_CODEC_Fxns->std.Init((const void *)&(enc_pvt->pInfo.params), enc_pvt->pBanks, &(enc_pvt->codec));

#ifndef IPPCORE_NO_SSE
  		ippCoreSetFlushToZero( 1, NULL );
#endif
  		enc_pvt->pcmStream.pcmType.bitPerSample = enc_pvt->pInfo.pcmType.bitPerSample;
		enc_pvt->pcmStream.nbytes = 160;
  		enc_pvt->pcmStream.pcmType.sample_frequency = enc_pvt->pInfo.pcmType.sample_frequency;
  		enc_pvt->bitStream.nbytes  = enc_pvt->maxbitsize;
  		enc_pvt->bitStream.pBuffer = enc_pvt->bitstream_buf;

		result = USC_CODEC_Fxns->std.Reinit((const void *)&(enc_pvt->pInfo.params.modes), enc_pvt->codec);

		enc_pvt->tail = 0;
	}
	
}


static void enc_uninit(MSFilter *f){

	int i;
	EncState *s=(EncState*)f->data;
	ms_bufferizer_destroy(s->bufferizer);
	ms_free(s);

	for(i = 0; i < enc_pvt->nbanks; i++) {
		if(enc_pvt->pBanks[i].pMem)
			ippsFree(enc_pvt->pBanks[i].pMem);	
		enc_pvt->pBanks[i].pMem=NULL;
	}
	if(enc_pvt->pBanks)
		ippsFree(enc_pvt->pBanks);
	free(enc_pvt);
}

static void enc_process(MSFilter *f){
	EncState *s=(EncState*)f->data;
	mblk_t *im;
	const int n_pck = s->pck_len/_PCK_LEN_MS;
	int pck=0;
	short ibuf[160];
	int obuf_len_byte = n_pck * _PCK_BITSTREAM_LEN_BYTES;

	while((im=ms_queue_get(f->inputs[0]))!=NULL){
		ms_bufferizer_put(s->bufferizer,im);
	}
	enc_pvt->tail = 0;
	while(ms_bufferizer_read(s->bufferizer,(uint8_t*)ibuf,sizeof(ibuf))==sizeof(ibuf)) {
		mblk_t *om=allocb(obuf_len_byte,0); 
		memset(om->b_wptr, 0 , obuf_len_byte);
		memcpy((enc_pvt->pcm_buf + enc_pvt->tail), ibuf, sizeof(ibuf));
		enc_pvt->tail += sizeof(ibuf)/2;
		for (pck=0;pck<n_pck;++pck){
			/* Copy the frame to workspace, then encode it */
            		enc_pvt->pcmStream.pBuffer = (char *)enc_pvt->pcm_buf;
			enc_pvt->bitStream.pBuffer = om->b_wptr;
			USC_CODEC_Fxns->Encode (enc_pvt->codec, &enc_pvt->pcmStream, &enc_pvt->bitStream);
			enc_pvt->tail -= 80;
            		if(enc_pvt->tail)
				memmove(enc_pvt->pcm_buf, enc_pvt->pcm_buf + 80, enc_pvt->tail * 2);
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
	int i;
	USC_Status result;
	ippStaticInit();
	if(!USC_CODEC_Fxns)
		USC_CODEC_Fxns = &USC_G729AFP_Fxns;
	dec_pvt = malloc(sizeof(struct g729_coder_pvt));
	memset((void *)dec_pvt, 0, sizeof(struct g729_coder_pvt));
	if(dec_pvt) {
		result = USC_CODEC_Fxns->std.GetInfo((USC_Handle)NULL, &(dec_pvt->pInfo));	
		//dec_pvt->pInfo.params.modes.bitrate = 0;
  		dec_pvt->pInfo.params.modes.bitrate = 8000;
		dec_pvt->pInfo.params.modes.truncate = 1;
  		dec_pvt->pInfo.params.direction = 1;

		/* dec_pvt->bitstream_buf = ippsMalloc_8s(size); */
		
		result = USC_CODEC_Fxns->std.NumAlloc((const void *)&(dec_pvt->pInfo.params), &(dec_pvt->nbanks));
		dec_pvt->pBanks = NULL;
		dec_pvt->pBanks = (USC_MemBank*)ippsMalloc_8u(sizeof(USC_MemBank)*(dec_pvt->nbanks));
		if(!dec_pvt->pBanks) {
			ms_warning("dec_init malloc failed allocating %d bytes", sizeof(USC_MemBank)*(dec_pvt->nbanks));
			return;
		}else{
			memset((void *)dec_pvt->pBanks, 0, sizeof(USC_MemBank)*(dec_pvt->nbanks));
		}
		dec_pvt->pInfo.params.direction = 1;
		result = USC_CODEC_Fxns->std.MemAlloc((const void *)&(dec_pvt->pInfo.params), dec_pvt->pBanks);

		for(i=0; i<dec_pvt->nbanks;i++) {
			if(dec_pvt->pBanks->nbytes>0){
				if(!(dec_pvt->pBanks[i].pMem = ippsMalloc_8u(dec_pvt->pBanks->nbytes))) {
					ms_warning("dec_init malloc failed allocating %d bytes", dec_pvt->pBanks->nbytes);
					return;
				}
			}else{
				ms_warning("dec_init dec_pvt->pBanks->nbytes %d\n", dec_pvt->pBanks->nbytes);
				return;
			}
		}

		//dec_pvt->outFrameSize = getOutFrameSize();

		/* pcm_buf ippsMalloc_8s(getOutFrameSize()); */

		dec_pvt->maxbitsize = dec_pvt->pInfo.maxbitsize;
		result = USC_CODEC_Fxns->std.Init((const void *)&(dec_pvt->pInfo.params), dec_pvt->pBanks, &(dec_pvt->codec));

#ifndef IPPCORE_NO_SSE
		ippCoreSetFlushToZero( 1, NULL );
#endif

		dec_pvt->bitStream.nbytes  = dec_pvt->maxbitsize;
		//dec_pvt->bitStream.bitrate = 0;
	  	dec_pvt->bitStream.bitrate = 8000;
	    dec_pvt->bitStream.frametype = 3;
	    dec_pvt->pcmStream.pBuffer = (char *)dec_pvt->pcm_buf;
		dec_pvt->pcmStream.pcmType.bitPerSample = 0;
	    dec_pvt->pcmStream.pcmType.sample_frequency = 0;

	    result = USC_CODEC_Fxns->std.Reinit((const void *)&(dec_pvt->pInfo.params.modes), dec_pvt->codec);

		dec_pvt->tail = 0;
	}
	return;
}
static void dec_uninit(MSFilter *f){

	int i;
	for(i = 0; i < dec_pvt->nbanks; i++) {
		if(dec_pvt->pBanks){
			if(dec_pvt->pBanks[i].pMem)
				ippsFree(dec_pvt->pBanks[i].pMem);	
			dec_pvt->pBanks[i].pMem=NULL;
		}
	}
	if(dec_pvt->pBanks)
		ippsFree(dec_pvt->pBanks);

	free(dec_pvt);
}

static void dec_process(MSFilter *f){
	mblk_t *im;
	mblk_t *om;
	//int i,byte,bit;
	//short _pck[_ADSP_G729_H_LEN + _PCK_BITSTREAM_LEN];
	dec_pvt->tail = 0;
	while((im=ms_queue_get(f->inputs[0]))!=NULL){
		while (im->b_wptr - im->b_rptr >= _PCK_BITSTREAM_LEN_BYTES){
			om=allocb(_PCK_SIZE,0); /* 80 * 2 */
			dec_pvt->bitStream.pBuffer = (char *)im->b_rptr;
			dec_pvt->pcmStream.pBuffer = (char *)(dec_pvt->pcm_buf + dec_pvt->tail);
			USC_CODEC_Fxns->Decode(dec_pvt->codec, &dec_pvt->bitStream,  &dec_pvt->pcmStream);
			/*copy g729 data from dec_pvt->pcmStream to om*/
			memcpy(om->b_wptr, dec_pvt->pcm_buf+dec_pvt->tail, _PCK_SIZE);
			om->b_wptr += _PCK_SIZE;
			ms_queue_put(f->outputs[0],om);
			im->b_rptr += 10;
			dec_pvt->tail += 80;
		}
		freemsg(im);
		dec_pvt->tail = 0;
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
	dec_uninit,
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
	.uninit=dec_uninit
};

#endif

MS_FILTER_DESC_EXPORT(ms_g729_dec_desc)
MS_FILTER_DESC_EXPORT(ms_g729_enc_desc)

