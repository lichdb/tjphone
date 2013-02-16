
/*
mediastreamer2 library - modular sound and video processing and streaming
Copyright (C) 2006  Simon MORLAT (simon.morlat@linphone.org)


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
#define NO_SCRATCH_MEMORY_USED

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mediastreamer2/msfilter.h"

#include "ipp_w7.h"
#include <ippcore.h>
#include <ipps.h>
#include <ippsc.h>
#include <owng723.h>
#include "usc_cdcs.h"
#include "usc.h"


#define FMT_PACKED 0
#define FMT_UNPACKED 1

#define MY_G723_1_FRAME_SIZE 24
#define MY_G723_1_SAMPLE_SIZE 240               

#define G723_RATE_63 0
#define G723_RATE_53 1

#define DEFAULT_SEND_RATE G723_RATE_63

static int defaultSendRate;
struct g723_coder_pvt {
	G723Encoder_Obj *encoder;
        G723Decoder_Obj *decoder;
        int sendRate;

        int maxbitsize;
        int inFrameSize;
        int outFrameSize;

	short pcm_buf[8000];
	unsigned char bitstream_buf[1000];

	int tail;
};



static USC_Fxns *USC_CODEC_Fxns = NULL;
static struct g723_coder_pvt *enc_pvt = NULL;
static struct g723_coder_pvt *dec_pvt = NULL;


typedef struct EncState{
	uint32_t ts;
	int pck_len;
	MSBufferizer *bufferizer; /* a queue */
} EncState;


static void enc_init(MSFilter *f){
	int i;
	int eSize;
	EncState *s=ms_new(EncState, 1);
	memset(s, 0 , sizeof(*s));
	s->ts=0;
	s->pck_len = 48;
	s->bufferizer=ms_bufferizer_new();
	f->data=s;

	enc_pvt = malloc(sizeof(struct g723_coder_pvt));
	if(enc_pvt) {
		apiG723Encoder_Alloc(&eSize);
		enc_pvt->encoder = (G723Encoder_Obj*)ippsMalloc_8u(eSize); 
		enc_pvt->decoder = NULL;
		enc_pvt->sendRate = defaultSendRate;

		// Init, no VAD or silence compression
                apiG723Encoder_Init(enc_pvt->encoder, 0);

		enc_pvt->tail = 0;
	}
	
}


static void enc_uninit(MSFilter *f){

	int i;
	EncState *s=(EncState*)f->data;
	ms_bufferizer_destroy(s->bufferizer);
	ms_free(s);

	if(enc_pvt->encoder != NULL)
		ippsFree(enc_pvt->encoder);
	free(enc_pvt);
}

static void enc_process(MSFilter *f){
	EncState *s=(EncState*)f->data;
	mblk_t *im;
	const int i;
	short ibuf[240];
	int frameSize, sampleSize;
	int obuf_len_byte = 24;

        switch(enc_pvt->sendRate) {
        case G723_RATE_63:
                frameSize = 24;
                break;
        case G723_RATE_53:
                frameSize = 20;
                break;
        default:
                break;
        }

	while((im=ms_queue_get(f->inputs[0]))!=NULL){
		ms_bufferizer_put(s->bufferizer,im);
	}
	enc_pvt->tail = 0;
	sampleSize = MY_G723_1_SAMPLE_SIZE;
	while(ms_bufferizer_read(s->bufferizer,(uint8_t*)ibuf,sizeof(ibuf))==sizeof(ibuf)) {
		mblk_t *om=allocb(obuf_len_byte,0); 
		memset(om->b_wptr, 0 , obuf_len_byte);
		memcpy((enc_pvt->pcm_buf + enc_pvt->tail), ibuf, sizeof(ibuf));
		enc_pvt->tail += sizeof(ibuf)/2;
		if(enc_pvt->tail < sampleSize)
			return;
		while(enc_pvt->tail >= sampleSize) {
			/* Copy the frame to workspace, then encode it */
			apiG723Encode(enc_pvt->encoder, enc_pvt->pcm_buf, enc_pvt->sendRate, om->b_wptr);
			enc_pvt->tail -= sampleSize;
                	if(enc_pvt->tail)
                        	memmove(enc_pvt->pcm_buf, enc_pvt->pcm_buf + sampleSize, enc_pvt->tail * 2);
			om->b_wptr+=frameSize;
		}
		mblk_set_timestamp_info(om,s->ts);
		ms_queue_put(f->outputs[0],om); /* one frame per packet */
		s->ts+=sizeof(ibuf)/sizeof(short);
	}
}

#ifdef _MSC_VER

MSFilterDesc ms_g723_enc_desc={
	MS_G723_ENC_ID,
	"MSG723Enc",
	"The G723.1 codec",
	MS_FILTER_ENCODER,
	"G723",
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

MSFilterDesc ms_g723_enc_desc={
	.id=MS_G723_ENC_ID,
	.name="MSG723Enc",
	.text="The G723.1 codec",
	.category=MS_FILTER_ENCODER,
	.enc_fmt="G723",
	.ninputs=1,
	.noutputs=1,
	.init=enc_init,
	.process=enc_process,
	.uninit=enc_uninit,
};

#endif

static void dec_init(MSFilter *f){
	int i;
	int dSize;
	defaultSendRate = DEFAULT_SEND_RATE;
	dec_pvt = malloc(sizeof(struct g723_coder_pvt));

	if(dec_pvt) {
		apiG723Decoder_Alloc(&dSize);
		dec_pvt->encoder = NULL;
		dec_pvt->decoder = (G723Decoder_Obj *)ippsMalloc_8u(dSize);

		apiG723Decoder_Init(dec_pvt->decoder, 0);

		dec_pvt->tail = 0;
	}
	return;
}
static void dec_uninit(MSFilter *f){

	int i;
	if(dec_pvt->decoder != NULL)
		ippsFree(dec_pvt->decoder);

	free(dec_pvt);
}

static void dec_process(MSFilter *f){
	mblk_t *im;
	mblk_t *om;
	int x, i;
        int frameInfo;
        int frameSize;
	int datalen;
        int sampleSize = MY_G723_1_SAMPLE_SIZE;
	while((im=ms_queue_get(f->inputs[0]))!=NULL){
		while (im->b_wptr - im->b_rptr > 0){
			frameInfo = im->b_rptr[0] & (short)0x0003;	
			switch(frameInfo) {
                	case 0: /* Active frame, high rate, 24 bytes */
                        	frameSize = 24;
                        	break;
                	case 1: /* Active frame, low rate, 20 bytes */
                        	frameSize = 20;
                        	break;
                	case 2: /* Sid Frame, 4 bytes */
                        	frameSize = 4;
                        	break;
                	default: /* untransmitted */
                        	frameSize = 1;
                        	break;
                	}
			datalen = im->b_wptr - im->b_rptr;
			if(frameSize>datalen){
				ms_warning("Received a G.723.1 frame that was %d bytes\n", datalen);
				return;
			}
			if(dec_pvt->tail + sampleSize < sizeof(dec_pvt->pcm_buf) / 2) {
				apiG723Decode(dec_pvt->decoder, im->b_rptr, 0, dec_pvt->pcm_buf + dec_pvt->tail);
			}else{
				return;
			}
			om=allocb(480,0);
			/*copy g723 data from dec_pvt->pcmStream to om*/
			memcpy(om->b_wptr, dec_pvt->pcm_buf+dec_pvt->tail, 480);
			om->b_wptr += 480;
			ms_queue_put(f->outputs[0],om);
			im->b_rptr += frameSize;
			dec_pvt->tail += sampleSize;
		}
		freemsg(im);
		dec_pvt->tail = 0;
	}
}

#ifdef _MSC_VER

MSFilterDesc ms_g723_dec_desc={
	MS_G723_DEC_ID,
	"MSG723Dec",
	"The G723.1 codec",
	MS_FILTER_DECODER,
	"G723",
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

MSFilterDesc ms_g723_dec_desc={
	.id=MS_G723_DEC_ID,
	.name="MSG723Dec",
	.text="The G723.1 codec",
	.category=MS_FILTER_DECODER,
	.enc_fmt="G723",
	.ninputs=1,
	.noutputs=1,
	.init=dec_init,
	.process=dec_process,
	.uninit=dec_uninit
};

#endif

MS_FILTER_DESC_EXPORT(ms_g723_dec_desc)
MS_FILTER_DESC_EXPORT(ms_g723_enc_desc)
