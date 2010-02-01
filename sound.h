#ifndef _SOUND_H_
#define _SOUND_H_

#include "common.h"

namespace space{

	//ƒ`ƒƒƒ“ƒlƒ‹‚Í‰½‚àŒ¾‚í‚È‚¯‚ê‚ÎŸŽè‚ÉŠ„‚è“–‚Ä‚é‚¯‚Ç”í‚ç‚È‚¢•ÛØ‚ª–³‚¢B
	class SDLMIXERCHUNK{
		Mix_Chunk *chunk;
		int channel;

	public:
		int get_channel() const {return channel;}
		int set_channel(int _ch=-1) {return channel=_ch;}

		void stop(){
			if(channel>=0){ Mix_HaltChannel(channel); }
			return;
		}
		void play(int num_of_repeat=0){
			stop();
			if(chunk) channel = Mix_PlayChannel(channel,chunk,num_of_repeat);
			else return;
			if(channel<0){
				char s[256];
				sprintf(s,"Can't create sound channel !!!(%s)",Mix_GetError()); 
				throw s;
			}
			return;
		}
		void free(){
			if(chunk){
				stop();
				Mix_FreeChunk(chunk);
				chunk = NULL;
			}
			return;
		}
		void load(std::string fname){
			free();
			chunk = Mix_LoadWAV(fname.c_str());
			if(!chunk){
				char s[256];
				sprintf(s,"Can't load sound file !!!(%s,%s)",fname.c_str(),Mix_GetError()); 
				throw s;
			}
			return;
		}
		void pause(){if(channel>=0)Mix_Pause(channel);}
		void resume(){if(channel>=0)Mix_Resume(channel);}

		void fadeout(int ms){Mix_FadeOutChannel(channel,ms);}
		void fadein(int ms,int num_of_repeat=-1){Mix_FadeInChannel(channel,chunk,num_of_repeat,ms);}

		int setChunkVolume(int _vol){ return Mix_VolumeChunk(chunk,_vol); }
		int getChunkVolume() const { return chunk->volume;}

		bool isPlaying(){return Mix_Playing(channel)>0;}
		bool isPaused(){return Mix_Paused(channel)>0;}
		Mix_Fading isFading(){if(channel<0)return MIX_NO_FADING;return Mix_FadingChannel(channel);}

		SDLMIXERCHUNK(int _ch=-1){channel=_ch;chunk=NULL;}
		virtual ~SDLMIXERCHUNK(){free();}
	};
}

#endif
