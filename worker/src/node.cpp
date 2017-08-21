// nodeとのやりとりをするnode-gypの動作部分
#include <nan.h>

using namespace v8;

static NAN_MODULE_INIT(Init) {
  puts("初期化してみる。");
}

NODE_MODULE(mediasoupGyp, Init);
