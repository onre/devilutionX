#include <cstddef>
#include <cstdint>
#include <string>

#include "all.h"
#include "options.h"
#include "paths.h"
#include "../3rdParty/Storm/Source/storm.h"

#if !SDL_VERSION_ATLEAST(2, 0, 4)
#include <queue>
#endif

#include "display.h"
#include "stubs.h"
#include "Radon.hpp"
#include <SDL.h>
#include <SDL_endian.h>
#include <SDL_mixer.h>
#include <smacker.h>

#include "DiabloUI/diabloui.h"

namespace devilution {

DWORD nLastError = 0;

namespace {

bool directFileAccess = false;
std::string *SBasePath = NULL;

} // namespace

#ifdef USE_SDL1
static bool IsSVidVideoMode = false;
#endif

radon::File &getIni()
{
	static radon::File ini(GetConfigPath() + "diablo.ini");
	return ini;
}

// Converts ASCII characters to lowercase
// Converts slash (0x2F) / backslash (0x5C) to system file-separator
unsigned char AsciiToLowerTable_Path[256] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
#ifdef _WIN32
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x5C,
#else
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
#endif
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
	0x40, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
#ifdef _WIN32
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
#else
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x5B, 0x2F, 0x5D, 0x5E, 0x5F,
#endif
	0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
	0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
	0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
	0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
	0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
	0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
	0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
	0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
	0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

BOOL SFileOpenFile(const char *filename, HANDLE *phFile)
{
	bool result = false;

	if (directFileAccess && SBasePath != NULL) {
		std::string path = *SBasePath + filename;
		for (std::size_t i = SBasePath->size(); i < path.size(); ++i)
			path[i] = AsciiToLowerTable_Path[static_cast<unsigned char>(path[i])];
		result = SFileOpenFileEx((HANDLE)0, path.c_str(), SFILE_OPEN_LOCAL_FILE, phFile);
	}

	if (!result && devilutionx_mpq != NULL) {
		result = SFileOpenFileEx((HANDLE)devilutionx_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}
	if (gbIsHellfire) {
		if (!result && hfopt2_mpq != NULL) {
			result = SFileOpenFileEx((HANDLE)hfopt2_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfopt1_mpq != NULL) {
			result = SFileOpenFileEx((HANDLE)hfopt1_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfvoice_mpq != NULL) {
			result = SFileOpenFileEx((HANDLE)hfvoice_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfmusic_mpq != NULL) {
			result = SFileOpenFileEx((HANDLE)hfmusic_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfbarb_mpq != NULL) {
			result = SFileOpenFileEx((HANDLE)hfbarb_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfbard_mpq != NULL) {
			result = SFileOpenFileEx((HANDLE)hfbard_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result && hfmonk_mpq != NULL) {
			result = SFileOpenFileEx((HANDLE)hfmonk_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
		if (!result) {
			result = SFileOpenFileEx((HANDLE)hellfire_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
		}
	}
	if (!result && patch_rt_mpq != NULL) {
		result = SFileOpenFileEx((HANDLE)patch_rt_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}
	if (!result && spawn_mpq != NULL) {
		result = SFileOpenFileEx((HANDLE)spawn_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}
	if (!result && diabdat_mpq != NULL) {
		result = SFileOpenFileEx((HANDLE)diabdat_mpq, filename, SFILE_OPEN_FROM_MPQ, phFile);
	}

	if (!result || !*phFile) {
		SDL_Log("%s: Not found: %s", __FUNCTION__, filename);
	}
	return result;
}

BOOL SBmpLoadImage(const char *pszFileName, SDL_Color *pPalette, BYTE *pBuffer, DWORD dwBuffersize, DWORD *pdwWidth, DWORD *dwHeight, DWORD *pdwBpp)
{
	HANDLE hFile;
	size_t size;
	PCXHeader pcxhdr;
	BYTE paldata[256][3];
	BYTE *dataPtr, *fileBuffer;
	BYTE byte;

	if (pdwWidth)
		*pdwWidth = 0;
	if (dwHeight)
		*dwHeight = 0;
	if (pdwBpp)
		*pdwBpp = 0;

	if (!pszFileName || !*pszFileName) {
		return false;
	}

	if (pBuffer && !dwBuffersize) {
		return false;
	}

	if (!pPalette && !pBuffer && !pdwWidth && !dwHeight) {
		return false;
	}

	if (!SFileOpenFile(pszFileName, &hFile)) {
		return false;
	}

	while (strchr(pszFileName, 92))
		pszFileName = strchr(pszFileName, 92) + 1;

	while (strchr(pszFileName + 1, 46))
		pszFileName = strchr(pszFileName, 46);

	// omit all types except PCX
	if (!pszFileName || strcasecmp(pszFileName, ".pcx")) {
		return false;
	}

	if (!SFileReadFile(hFile, &pcxhdr, 128, 0, 0)) {
		SFileCloseFile(hFile);
		return false;
	}

	int width = SDL_SwapLE16(pcxhdr.Xmax) - SDL_SwapLE16(pcxhdr.Xmin) + 1;
	int height = SDL_SwapLE16(pcxhdr.Ymax) - SDL_SwapLE16(pcxhdr.Ymin) + 1;

	// If the given buffer is larger than width * height, assume the extra data
	// is scanline padding.
	//
	// This is useful because in SDL the pitch size is often slightly larger
	// than image width for efficiency.
	const int x_skip = dwBuffersize / height - width;

	if (pdwWidth)
		*pdwWidth = width;
	if (dwHeight)
		*dwHeight = height;
	if (pdwBpp)
		*pdwBpp = pcxhdr.BitsPerPixel;

	if (!pBuffer) {
		SFileSetFilePointer(hFile, 0, NULL, DVL_FILE_END);
		fileBuffer = NULL;
	} else {
		const auto pos = SFileGetFilePointer(hFile);
		const auto end = SFileSetFilePointer(hFile, 0, DVL_FILE_END);
		const auto begin = SFileSetFilePointer(hFile, pos, DVL_FILE_BEGIN);
		size = end - begin;
		fileBuffer = (BYTE *)malloc(size);
	}

	if (fileBuffer) {
		SFileReadFile(hFile, fileBuffer, size, 0, 0);
		dataPtr = fileBuffer;

		for (int j = 0; j < height; j++) {
			for (int x = 0; x < width; dataPtr++) {
				byte = *dataPtr;
				if (byte < 0xC0) {
					*pBuffer = byte;
					pBuffer++;
					x++;
					continue;
				}
				dataPtr++;

				for (int i = 0; i < (byte & 0x3F); i++) {
					*pBuffer = *dataPtr;
					pBuffer++;
					x++;
				}
			}
			// Skip the pitch padding.
			pBuffer += x_skip;
		}

		free(fileBuffer);
	}

	if (pPalette && pcxhdr.BitsPerPixel == 8) {
		const auto pos = SFileSetFilePointer(hFile, -768, DVL_FILE_CURRENT);
		if (pos == static_cast<std::uint64_t>(-1)) {
			SDL_Log("SFileSetFilePointer error: %ud", (unsigned int)SErrGetLastError());
		}
		SFileReadFile(hFile, paldata, 768, 0, NULL);

		for (int i = 0; i < 256; i++) {
			pPalette[i].r = paldata[i][0];
			pPalette[i].g = paldata[i][1];
			pPalette[i].b = paldata[i][2];
#ifndef USE_SDL1
			pPalette[i].a = SDL_ALPHA_OPAQUE;
#endif
		}
	}

	SFileCloseFile(hFile);

	return true;
}

bool getIniBool(const char *sectionName, const char *keyName, bool defaultValue)
{
	char string[2];

	if (!getIniValue(sectionName, keyName, string, 2))
		return defaultValue;

	return strtol(string, NULL, 10) != 0;
}

bool getIniValue(const char *sectionName, const char *keyName, char *string, int stringSize, const char *defaultString)
{
	strncpy(string, defaultString, stringSize);

	radon::Section *section = getIni().getSection(sectionName);
	if (!section)
		return false;

	radon::Key *key = section->getKey(keyName);
	if (!key)
		return false;

	std::string value = key->getStringValue();

	if (string != NULL)
		strncpy(string, value.c_str(), stringSize);

	return true;
}

void setIniValue(const char *sectionName, const char *keyName, const char *value, int len)
{
	radon::File &ini = getIni();

	radon::Section *section = ini.getSection(sectionName);
	if (!section) {
		ini.addSection(sectionName);
		section = ini.getSection(sectionName);
	}

	std::string stringValue(value, len ? len : strlen(value));

	radon::Key *key = section->getKey(keyName);
	if (!key) {
		section->addKey(radon::Key(keyName, stringValue));
	} else {
		key->setValue(stringValue);
	}
}

void SaveIni()
{
	getIni().saveToFile();
}

int getIniInt(const char *keyname, const char *valuename, int defaultValue)
{
	char string[10];
	if (!getIniValue(keyname, valuename, string, sizeof(string))) {
		return defaultValue;
	}

	return strtol(string, NULL, sizeof(string));
}

void setIniInt(const char *keyname, const char *valuename, int value)
{
	char str[10];
	sprintf(str, "%d", value);
	setIniValue(keyname, valuename, str);
}

double SVidFrameEnd;
double SVidFrameLength;
char SVidAudioDepth;
double SVidVolume;
BYTE SVidLoop;
smk SVidSMK;
SDL_Color SVidPreviousPalette[256];
SDL_Palette *SVidPalette;
SDL_Surface *SVidSurface;
BYTE *SVidBuffer;
unsigned long SVidWidth, SVidHeight;

#if SDL_VERSION_ATLEAST(2, 0, 4)
SDL_AudioDeviceID deviceId;
static bool HaveAudio()
{
	return deviceId != 0;
}
#else
static bool HaveAudio()
{
	return SDL_GetAudioStatus() != SDL_AUDIO_STOPPED;
}
#endif

void SVidRestartMixer()
{
	if (Mix_OpenAudio(22050, AUDIO_S16LSB, 2, 1024) < 0) {
		SDL_Log(Mix_GetError());
	}
	Mix_AllocateChannels(25);
	Mix_ReserveChannels(1);
}

#if !SDL_VERSION_ATLEAST(2, 0, 4)
struct AudioQueueItem {
	unsigned char *data;
	unsigned long len;
	const unsigned char *pos;
};

class AudioQueue {
public:
	static void Callback(void *userdata, Uint8 *out, int out_len)
	{
		static_cast<AudioQueue *>(userdata)->Dequeue(out, out_len);
	}

	void Subscribe(SDL_AudioSpec *spec)
	{
		spec->userdata = this;
		spec->callback = AudioQueue::Callback;
	}

	void Enqueue(const unsigned char *data, unsigned long len)
	{
#if SDL_VERSION_ATLEAST(2, 0, 4)
		SDL_LockAudioDevice(deviceId);
		EnqueueUnsafe(data, len);
		SDL_UnlockAudioDevice(deviceId);
#else
		SDL_LockAudio();
		EnqueueUnsafe(data, len);
		SDL_UnlockAudio();
#endif
	}

	void Clear()
	{
		while (!queue_.empty())
			Pop();
	}

private:
	void EnqueueUnsafe(const unsigned char *data, unsigned long len)
	{
		AudioQueueItem item;
		item.data = new unsigned char[len];
		memcpy(item.data, data, len * sizeof(item.data[0]));
		item.len = len;
		item.pos = item.data;
		queue_.push(item);
	}

	void Dequeue(Uint8 *out, int out_len)
	{
		SDL_memset(out, 0, sizeof(out[0]) * out_len);
		AudioQueueItem *item;
		while ((item = Next()) != NULL) {
			if (static_cast<unsigned long>(out_len) <= item->len) {
				SDL_MixAudio(out, item->pos, out_len, SDL_MIX_MAXVOLUME);
				item->pos += out_len;
				item->len -= out_len;
				return;
			}

			SDL_MixAudio(out, item->pos, item->len, SDL_MIX_MAXVOLUME);
			out += item->len;
			out_len -= item->len;
			Pop();
		}
	}

	AudioQueueItem *Next()
	{
		while (!queue_.empty() && queue_.front().len == 0)
			Pop();
		if (queue_.empty())
			return NULL;
		return &queue_.front();
	}

	void Pop()
	{
		delete[] queue_.front().data;
		queue_.pop();
	}

	std::queue<AudioQueueItem> queue_;
};

static AudioQueue *sVidAudioQueue = new AudioQueue();
#endif

void SVidPlayBegin(const char *filename, int a2, int a3, int a4, int a5, int flags, HANDLE *video)
{
	if (flags & 0x10000 || flags & 0x20000000) {
		return;
	}

	SVidLoop = false;
	if (flags & 0x40000)
		SVidLoop = true;
	bool enableVideo = !(flags & 0x100000);
	bool enableAudio = !(flags & 0x1000000);
	//0x8 // Non-interlaced
	//0x200, 0x800 // Upscale video
	//0x80000 // Center horizontally
	//0x800000 // Edge detection
	//0x200800 // Clear FB

	SFileOpenFile(filename, video);

	int bytestoread = SFileGetFileSize(*video, 0);
	SVidBuffer = DiabloAllocPtr(bytestoread);
	SFileReadFile(*video, SVidBuffer, bytestoread, NULL, 0);

	SVidSMK = smk_open_memory(SVidBuffer, bytestoread);
	if (SVidSMK == NULL) {
		return;
	}

	unsigned char channels[7], depth[7];
	unsigned long rate[7];
	smk_info_audio(SVidSMK, NULL, channels, depth, rate);
	if (enableAudio && depth[0] != 0) {
		smk_enable_audio(SVidSMK, 0, enableAudio);
		SDL_AudioSpec audioFormat;
		SDL_zero(audioFormat);
		audioFormat.freq = rate[0];
		audioFormat.format = depth[0] == 16 ? AUDIO_S16SYS : AUDIO_U8;
		SVidAudioDepth = depth[0];
		audioFormat.channels = channels[0];

		SVidVolume = sgOptions.Audio.nSoundVolume - VOLUME_MIN;
		SVidVolume /= -VOLUME_MIN;

		Mix_CloseAudio();

#if SDL_VERSION_ATLEAST(2, 0, 4)
		deviceId = SDL_OpenAudioDevice(NULL, 0, &audioFormat, NULL, 0);
		if (deviceId == 0) {
			ErrSdl();
		}

		SDL_PauseAudioDevice(deviceId, 0); /* start audio playing. */
#else
		sVidAudioQueue->Subscribe(&audioFormat);
		if (SDL_OpenAudio(&audioFormat, NULL) != 0) {
			ErrSdl();
		}
		SDL_PauseAudio(0);
#endif
	}

	unsigned long nFrames;
	smk_info_all(SVidSMK, NULL, &nFrames, &SVidFrameLength);
	smk_info_video(SVidSMK, &SVidWidth, &SVidHeight, NULL);

	smk_enable_video(SVidSMK, enableVideo);
	smk_first(SVidSMK); // Decode first frame

	smk_info_video(SVidSMK, &SVidWidth, &SVidHeight, NULL);
#ifndef USE_SDL1
	if (renderer) {
		SDL_DestroyTexture(texture);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SVidWidth, SVidHeight);
		if (texture == NULL) {
			ErrSdl();
		}
		if (SDL_RenderSetLogicalSize(renderer, SVidWidth, SVidHeight) <= -1) {
			ErrSdl();
		}
	}
#else
	// Set the video mode close to the SVid resolution while preserving aspect ratio.
	{
		const SDL_Surface *display = SDL_GetVideoSurface();
		IsSVidVideoMode = (display->flags & (SDL_FULLSCREEN | SDL_NOFRAME)) != 0;

		if (IsSVidVideoMode) {
			/* Get available fullscreen/hardware modes */
			SDL_Rect **modes = SDL_ListModes(NULL, display->flags);

			/* Check is there are any modes available */
			if (modes == (SDL_Rect **)0) {
				IsSVidVideoMode = false;
			}

			/* Check if our resolution is restricted */
			if (modes != (SDL_Rect **)-1) {
				// Search for a usable video mode
				bool UsableModeFound = false;
				for (int i = 0; modes[i]; i++) {
					if (modes[i]->w == SVidWidth || modes[i]->h == SVidHeight) {
						UsableModeFound = true;
						break;
					}
				}
				IsSVidVideoMode = UsableModeFound;
			}
		}

		if (IsSVidVideoMode) {
			int w, h;
			if (display->w * SVidWidth > display->h * SVidHeight) {
				w = SVidWidth;
				h = SVidWidth * display->h / display->w;
			} else {
				w = SVidHeight * display->w / display->h;
				h = SVidHeight;
			}
			SetVideoMode(w, h, display->format->BitsPerPixel, display->flags);
		}
	}
#endif
	memcpy(SVidPreviousPalette, orig_palette, sizeof(SVidPreviousPalette));

	// Copy frame to buffer
	SVidSurface = SDL_CreateRGBSurfaceWithFormatFrom(
	    (unsigned char *)smk_get_video(SVidSMK),
	    SVidWidth,
	    SVidHeight,
	    8,
	    SVidWidth,
	    SDL_PIXELFORMAT_INDEX8);
	if (SVidSurface == NULL) {
		ErrSdl();
	}

	SVidPalette = SDL_AllocPalette(256);
	if (SVidPalette == NULL) {
		ErrSdl();
	}
	if (SDLC_SetSurfaceColors(SVidSurface, SVidPalette) <= -1) {
		ErrSdl();
	}

	SVidFrameEnd = SDL_GetTicks() * 1000 + SVidFrameLength;
	SDL_FillRect(GetOutputSurface(), NULL, 0x000000);
}

BOOL SVidLoadNextFrame()
{
	SVidFrameEnd += SVidFrameLength;

	if (smk_next(SVidSMK) == SMK_DONE) {
		if (!SVidLoop) {
			return false;
		}

		smk_first(SVidSMK);
	}

	return true;
}

unsigned char *SVidApplyVolume(const unsigned char *raw, unsigned long rawLen)
{
	unsigned char *scaled = (unsigned char *)malloc(rawLen);

	if (SVidAudioDepth == 16) {
		for (unsigned long i = 0; i < rawLen / 2; i++)
			((Sint16 *)scaled)[i] = ((Sint16 *)raw)[i] * SVidVolume;
	} else {
		for (unsigned long i = 0; i < rawLen; i++)
			scaled[i] = raw[i] * SVidVolume;
	}

	return (unsigned char *)scaled;
}

BOOL SVidPlayContinue(void)
{
	if (smk_palette_updated(SVidSMK)) {
		SDL_Color colors[256];
		const unsigned char *palette_data = smk_get_palette(SVidSMK);

		for (int i = 0; i < 256; i++) {
			colors[i].r = palette_data[i * 3 + 0];
			colors[i].g = palette_data[i * 3 + 1];
			colors[i].b = palette_data[i * 3 + 2];
#ifndef USE_SDL1
			colors[i].a = SDL_ALPHA_OPAQUE;
#endif

			orig_palette[i].r = palette_data[i * 3 + 0];
			orig_palette[i].g = palette_data[i * 3 + 1];
			orig_palette[i].b = palette_data[i * 3 + 2];
		}
		memcpy(logical_palette, orig_palette, sizeof(logical_palette));

		if (SDLC_SetSurfaceAndPaletteColors(SVidSurface, SVidPalette, colors, 0, 256) <= -1) {
			SDL_Log(SDL_GetError());
			return false;
		}
	}

	if (SDL_GetTicks() * 1000 >= SVidFrameEnd) {
		return SVidLoadNextFrame(); // Skip video and audio if the system is to slow
	}

	if (HaveAudio()) {
		unsigned long len = smk_get_audio_size(SVidSMK, 0);
		unsigned char *audio = SVidApplyVolume(smk_get_audio(SVidSMK, 0), len);
#if SDL_VERSION_ATLEAST(2, 0, 4)
		if (SDL_QueueAudio(deviceId, audio, len) <= -1) {
			SDL_Log(SDL_GetError());
			return false;
		}
#else
		sVidAudioQueue->Enqueue(audio, len);
#endif
		free(audio);
	}

	if (SDL_GetTicks() * 1000 >= SVidFrameEnd) {
		return SVidLoadNextFrame(); // Skip video if the system is to slow
	}

#ifndef USE_SDL1
	if (renderer) {
		if (SDL_BlitSurface(SVidSurface, NULL, GetOutputSurface(), NULL) <= -1) {
			SDL_Log(SDL_GetError());
			return false;
		}
	} else
#endif
	{
		SDL_Surface *output_surface = GetOutputSurface();
		int factor;
		int wFactor = output_surface->w / SVidWidth;
		int hFactor = output_surface->h / SVidHeight;
		if (wFactor > hFactor && (unsigned int)output_surface->h > SVidHeight) {
			factor = hFactor;
		} else {
			factor = wFactor;
		}
		const Uint16 scaledW = SVidWidth * factor;
		const Uint16 scaledH = SVidHeight * factor;
		const Sint16 scaledX = (output_surface->w - scaledW) / 2;
		const Sint16 scaledY = (output_surface->h - scaledH) / 2;

		SDL_Rect pal_surface_offset = { scaledX, scaledY, scaledW, scaledH };
		if (factor == 1) {
			if (SDL_BlitSurface(SVidSurface, NULL, output_surface, &pal_surface_offset) <= -1) {
				ErrSdl();
			}
		} else {
#ifdef USE_SDL1
			SDL_Surface *tmp = SDL_ConvertSurface(SVidSurface, ghMainWnd->format, 0);
#else
			Uint32 format = SDL_GetWindowPixelFormat(ghMainWnd);
			SDL_Surface *tmp = SDL_ConvertSurfaceFormat(SVidSurface, format, 0);
#endif
			if (SDL_BlitScaled(tmp, NULL, output_surface, &pal_surface_offset) <= -1) {
				SDL_Log(SDL_GetError());
				return false;
			}
			SDL_FreeSurface(tmp);
		}
	}

	RenderPresent();

	double now = SDL_GetTicks() * 1000;
	if (now < SVidFrameEnd) {
		SDL_Delay((SVidFrameEnd - now) / 1000); // wait with next frame if the system is too fast
	}

	return SVidLoadNextFrame();
}

void SVidPlayEnd(HANDLE video)
{
	if (HaveAudio()) {
#if SDL_VERSION_ATLEAST(2, 0, 4)
		SDL_ClearQueuedAudio(deviceId);
		SDL_CloseAudioDevice(deviceId);
		deviceId = 0;
#else
		SDL_CloseAudio();
		sVidAudioQueue->Clear();
#endif
		SVidRestartMixer();
	}

	if (SVidSMK)
		smk_close(SVidSMK);

	if (SVidBuffer) {
		mem_free_dbg(SVidBuffer);
		SVidBuffer = NULL;
	}

	SDL_FreePalette(SVidPalette);
	SVidPalette = NULL;

	SDL_FreeSurface(SVidSurface);
	SVidSurface = NULL;

	SFileCloseFile(video);
	video = NULL;

	memcpy(orig_palette, SVidPreviousPalette, sizeof(orig_palette));
#ifndef USE_SDL1
	if (renderer) {
		SDL_DestroyTexture(texture);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, gnScreenWidth, gnScreenHeight);
		if (texture == NULL) {
			ErrSdl();
		}
		if (renderer && SDL_RenderSetLogicalSize(renderer, gnScreenWidth, gnScreenHeight) <= -1) {
			ErrSdl();
		}
	}
#else
	if (IsSVidVideoMode)
		SetVideoModeToPrimary(IsFullScreen(), gnScreenWidth, gnScreenHeight);
#endif
}

DWORD SErrGetLastError()
{
	return nLastError;
}

void SErrSetLastError(DWORD dwErrCode)
{
	nLastError = dwErrCode;
}

BOOL SFileSetBasePath(const char *path)
{
	if (SBasePath == NULL)
		SBasePath = new std::string;
	*SBasePath = path;
	return true;
}

BOOL SFileEnableDirectAccess(BOOL enable)
{
	directFileAccess = enable;
	return true;
}
} // namespace devilution
