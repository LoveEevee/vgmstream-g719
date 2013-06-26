#include "../vgmstream.h"

#ifdef VGM_USE_MAIATRAC3PLUS
#include "maiatrac3plus.h"
#include "coding.h"
#include "../util.h"

void decode_at3(VGMSTREAM * vgmstream, 
        sample * outbuf, int channelspacing, int32_t samples_to_do, int channel) {
    VGMSTREAMCHANNEL *ch = &vgmstream->ch[0];
    maiatrac3plus_codec_data *data = vgmstream->codec_data;
    int i;

	if ((0 == vgmstream->samples_into_block || data->samples_discard == vgmstream->samples_into_block) && 0 == channel)
    {
        uint8_t code_buffer[0x8000];
		int blocks_to_decode = 1;
		int max_blocks_to_decode = (ch->offset - ch->channel_start_offset) / (vgmstream->interleave_block_size * vgmstream->channels) + 1;
		if (data->samples_discard) blocks_to_decode = 8;
		if (blocks_to_decode > max_blocks_to_decode) blocks_to_decode = max_blocks_to_decode;
		while (blocks_to_decode--) {
			vgmstream->ch[0].streamfile->read(ch->streamfile, code_buffer, ch->offset - blocks_to_decode * vgmstream->interleave_block_size * vgmstream->channels, vgmstream->interleave_block_size * vgmstream->channels);
			Atrac3plusDecoder_decodeFrame(data->handle, code_buffer, vgmstream->interleave_block_size * vgmstream->channels, &data->channels, (void**)&data->buffer);
		}
		data->samples_discard = 0;
    }

    for (i = 0; i < samples_to_do; i++)
    {
        outbuf[i*channelspacing] = data->buffer[(vgmstream->samples_into_block+i)*data->channels+channel];
    }
}

#endif
