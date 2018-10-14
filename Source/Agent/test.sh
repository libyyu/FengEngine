
CURL_PATH=../../3rd/curl
#./configure  --disable-shared --enable-static --without-zlib --without-libidn --without-ssl --without-librtmp --without-gnutls --without-nss --without-libssh2 --without-winidn --disable-rtsp --disable-ldap --disable-ldaps --disable-ipv6
MODULE_NAME=libAgent
g++ -c \
	-I./ \
	-I$CURL_PATH/include \
	HTTPRequest.cpp \
	exp_HTTPRequest.cpp \
	-std=c++11 -static \
	-DCURL_STATICLIB
ar cr ${MODULE_NAME}.a exp_HTTPRequest.o HTTPRequest.o
rm -rf *.o

MODULE_NAME=test
g++ \
	-I./ \
	-I$CURL_PATH/include \
	-L./ \
	-L../../3rd/curl/lib/macosx \
	HTTPDemo.cpp \
	-o ${MODULE_NAME} -std=c++11 \
	-DCURL_STATICLIB \
	-lcurl -lldap -lAgent