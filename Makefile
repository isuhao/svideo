CFLAGS+=-Wall \
		-O2 \
		-std=c99 \
		  
FFMPEGLIB_WIN=\
ffmpeg\lib\win\libavcodec.dll.a \
ffmpeg\lib\win\libavformat.dll.a \
ffmpeg\lib\win\libavutil.dll.a \
ffmpeg\lib\win\libswscale.dll.a \
ffmpeg\lib\win\libswresample.dll.a

	
D3DLIB = \
-ld3d9 \
-lole32 \
-lgdi32
		
WIN_SRC=\
svideop.c \
render.c \
render_gdi.c \
render_d3d.c \
svideo_convert.c \
winapi.c 


LINUX_INCLUD=\
-Iffmpeg/include \
-Iusr/include/va \
-Isdl/include

LINUX_LIB=\
-Lffmpeg/lib/linux \
-lavcodec \
-lavformat \
-lavutil \
-lswscale \
-lswresample \
-lSDL2 \
-lva \
-lva-x11 \
-lX11

		

HVPLAYER_SRC=\
linux/hvplayer2.c \
linux/vaapi.c \
linux/vad.c \
linux/my_utils.c \
linux/socket.c \
linux/h264_recv.c
			
all:
	echo 'make win or make linux'

win:svideo.dll
linux:libHvDeviceVideo.so hvplayer

svideo.dll:svideop.exe svideo.c 
	gcc $(CFLAGS) -shared -Wl,--output-def,svideo.def,--out-implib,svideo.a -o $@ svideo.c 
	lib /machine:ix86 /def:svideo.def

svideop.exe:$(WIN_SRC)
	gcc $(CFLAGS)  -Iffmpeg/include -o $@ $^ $(FFMPEGLIB_WIN) $(D3DLIB)


hvplayer:$(HVPLAYER_SRC)
	gcc $(CFLAGS) $(LINUX_INCLUDE) -o $@ $^ $(LINUX_LIB)

libHvDeviceVideo.so:linux/hvvideo.c
	gcc $(CFLAGS) -fPIC -shared -o $@ $^ -lpthread
	
.PHONY:clean

clean:
	-rm *.dll
	-rm *.so
	-rm *.a
	-rm *.def
	-rm *.lib
	-rm *.exe

.PHONY:install

install:
	-mkdir ./bin
	-mkdir ./bin/lib
	-mkdir ./bin/include
	-mv svideo.dll ./bin/lib
	-mv svideo.a ./bin/lib
	-mv svideo.lib ./bin/lib
	-mv libHvDeviceVideo.so ./bin/lib
	-cp hvtype.h ./bin/include
	-cp svideo_convert.h ./bin/include
	-cp svideo_rtsp.h ./bin/include
	-rm svideo.def
	-rm *.exp
uninstall:
	-rm dir -rf ./bin
