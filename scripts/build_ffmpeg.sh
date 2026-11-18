#!/bin/bash
# Builds a small static FFmpeg (no external codec libraries) for the PC build of S3-Springboard.
# Run inside an MSYS2 UCRT64 shell:  bash scripts/build_ffmpeg.sh
# Result: third_party_bin/ffmpeg/{include,lib}; CMake picks it up automatically.
set -e
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
ROOTW="$(cygpath -m "$ROOT" 2>/dev/null || echo "$ROOT")"
VER=7.1.1
WORK="${TMPDIR:-/tmp}/ffmpeg-build"
mkdir -p "$WORK"
cd "$WORK"
if [ ! -d "ffmpeg-$VER" ]; then
    curl -L -o "ffmpeg-$VER.tar.xz" "https://ffmpeg.org/releases/ffmpeg-$VER.tar.xz"
    tar xf "ffmpeg-$VER.tar.xz"
fi
cd "ffmpeg-$VER"
./configure \
    --prefix="$ROOTW/third_party_bin/ffmpeg" \
    --arch=x86_64 --target-os=mingw32 \
    --enable-static --disable-shared --disable-programs --disable-doc --disable-debug \
    --disable-autodetect --disable-everything --disable-network \
    --disable-avdevice --disable-avfilter --disable-postproc \
    --enable-protocol=file \
    --enable-demuxer=mov,matroska,avi,mp3,flac,ogg,wav,aac,flv,mpegts,mpegps,asf,ape,ac3,eac3,h264,hevc,mjpeg,gif,aiff,caf,wv \
    --enable-decoder=h264,hevc,vp8,vp9,mpeg4,mpeg2video,mpeg1video,msmpeg4v3,msmpeg4v2,wmv1,wmv2,wmv3,mjpeg,png,gif,aac,aac_latm,mp3,mp3float,mp2,mp2float,flac,vorbis,opus,alac,ac3,eac3,wmav1,wmav2,ape,wavpack,pcm_s16le,pcm_s24le,pcm_s32le,pcm_f32le,pcm_u8,pcm_s16be,pcm_alaw,pcm_mulaw \
    --enable-parser=h264,hevc,vp8,vp9,mpeg4video,mpegvideo,aac,aac_latm,mpegaudio,flac,vorbis,opus,ac3,mjpeg,png,gif \
    --enable-swscale --enable-swresample
make -j"$(nproc)"
make install
echo "FFmpeg installed to $ROOTW/third_party_bin/ffmpeg"
