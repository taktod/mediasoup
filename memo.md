やったこと

ライブラリを外部依存にしてみた。これで余計なコンパイルはしらなくなったので
ちょっとコンパイルが早くなった。

c++のincludeまわりを相対パスに変更した。

uvから経過時間を取得してたのを独自に計算するように変更。
(これでwindowsでは動作しなくなった。まぁwindowsのAPIで時間とるようにしてもいいけど
テストするのと使いたいのがMacOSかLinuxなので、まぁいいや。)

uv_default_loopをつかって動作するように変更。
これを使うことでnode-gyp化したときにnodeのlibuvをベースに動作可能になったはず。

mganekoさんのmediasoup_sampleのコードを含めといた。
これでnpm installで全コンパイルではなく、make Releaseで変更点だけコンパイルして
動作テストが可能になった。

h264やopusのデータを通信から取り出して保存してみた。
matroskaの中間フレームで分割にして動作させたらうまく動作した。

やっときたいこと。

node-gyp化しておきたい。
h264やopusのデータを別のものに差し替えて応答返せるようにしたい。
-> とりあえず自分の映像をそのまま打ち返してみようと思う。
 -> receiverが取得できないから、動作しなかった。これはちょっとやっかいだな。