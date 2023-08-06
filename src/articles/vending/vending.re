= 精算機作るよ！！
@<author>{AIメスガキには魂がない, lrks;twitter:__lrks__}
//profile{
今期は白聖女と黒牧師です。
TODOみて
//}
//lead{
「精算機作るよ！」の続編、厳しく改善致します。という内容です。
//}


== ざぁこ@<emoji>{heart}ザコ精◯生産しろ@<emoji>{heart}
「おしながき」「さいこうのすじがき@<fn>{fujisaka2023}」が卑猥な意味に見える (SMEKHR)
//footnote[fujisaka2023][藤坂リリック，さいこうのすじがき，茜新社，2023．]

さて、1年前のC100では@<img>{fusion360}の設計に基づき精@<ruby>{◯, 算機}を生産し、当時頒布したkmm?内で「精算機作るよ！」として仕様と開発のなんやかんやをまとめました。
//subfig[生産した精算機]{
//image[fusion360][設計図][scale=0.4]
//image[c100][実運用（画像はTwitter(X) @sugerestより)][scale=0.5]
//}
「精算機作るよ！」の概要としては、

 * コインメック（自販機向け硬貨選別機）とビルバリ（紙幣識別機）を制御したいなぁ
 * でも制御したところで使い道無いなぁ…そうだコミケで精算機として使おう！
 * でも仕様が分からないなぁ…コインメックとビルバリは券売機にも使われている！中古の券売機なら買える値段なのでこれを買って解析しよう！
 * でも実はサードパーティから販売されている「コインメックとビルバリの制御基板」を制御するサンプルコードから仕様が分かったかも

という内容です。
実際C100会場で@<img>{c100}のように実用できる状態まで持ってこれましたが、改善が必要な点は見つかりました。
具体的には次のとおりです。

: 設営・撤収が大変すぎる問題
  設営の際、コインメックとビルバリは分離した状態で搬入し現地で組み立てて精算機として利用できるようにしていました。
  撤収の際も分解の必要があり、これでは手間がかかります。
  そこで今回は組み立てた状態で搬入すべく、コンパクトにする改良を行いました。
: 機能増やしたい問題
  製品として販売されている精算機、または券売機や自販機と比べると機能不足を感じています。
  そりゃ手作りだから…というのはありますが多少マシになるようにしたく、今回は主にソフトウェアの改良を行いました。
: 前髪すかすか問題
  現状では前面が覆われておらず裏のごちゃごちゃした配線が丸見えでした。
  これを隠すためと、アクセプタンスマークを提示したりの案内のためパネルを作ります。
  当初は外注でアクリル+UV印刷を想定していましたが値段が高く、今回はハンドメイドで3Dプリンタ造形+熱転写でパネルを作成しました。

本稿ではこの問題の詳細と改善の内容について述べます。
また、さらなる解析のため券売機を追加購入しましたので、この輸送の様子についても紹介します。


== 設営・撤収が大変すぎる問題
この精算機は持ち運びを想定していませんでした。
精算機を構成する代表的なコンポーネントには次のものが含まれ、これらは分離された状態で搬入されます。

 * コインメック
 * ビルバリ
 * バーコードリーダー（QRコード決済用）
 * Square Terminal（クレジットカード・電子マネー決済用）
 * Square Terminal専用ハブとACアダプタ
 * コインメック・ビルバリ制御基板およびACアダプタ
 * PC（「バーコードリーダー」「Square Terminal」「コインメック・ビルバリ制御基板」の制御、iPadへのWeb UI提供用）
 * iPad（PC操作、テザリング用）
 * ポータブル電源

構成図としては図TODOとなり、これらを現地で組み立て・配線を行います。
設営・撤収時に手間となり、厳しく改善致します。としたいです。

具体的な改良点は次から述べていきますが、内容としては
組み立てた状態で搬入すべくダンボールに入る程度に小型化する、
またコンポーネント自体を減らすためにUSB Gadget Driverを@<bou>{試そうとした}記録です。

=== 小型化
組み立てた状態で運搬するため、その辺にあるダンボールに入らないかな？と試した結果、
FlexiSpotのCPUホルダーが入っていた空箱@<fn>{flexispot}にほぼ収まることが分かりました。
しかも、Wフルートとなっており頑丈さも期待できます。
//footnote[flexispot][幅20cm・長さ25cm・高さ45cmほど]

ただ、高さだけが微妙に収まりません。
そこで、精算機本体の高さを数cm下げることにしました。
お釣りの取り出し口として確保していた空間を狭めたことで実現はできましたが、
狭めすぎてお釣りが取り出しにくいかもしれませんが、指は入るので大丈夫でしょう。

また、PCも同じ箱、というか精算機のデッドスペースに含めることにしました。
従来ではノートPCを利用していましたが、ここではRaspberry Pi 4に置き換えることにします。

そもそも最初からRaspberry Pi 4を使いたかったのですが、
在庫的に以前はまともな販売店からの入手が難しく、ヤフオクやAmazonの転売ルートから購入するのも気が引けてノートPCを使っていました。
求める要件としては次のとおりであり、正直これならRaspberry Pi 4でなくとも満たす製品はあります。

 * 10cm角に収まる程度には小型
 * USB Gadget Driverが使えること（後述）
 * できればEthernet、Wi-Fiが備え付けであること（もし使うことになっても外付けの機器を増やしたくない）
 * できればUSB Type-Aを3ポート以上備えるもの（USBハブを増やしたくない）

ただ、すべて海外からの輸入となるため、送料や円安であることを含めると割高に思えてしまいます。
また、Raspberry Pi 4ではありがたいことに有志のおかげでさまざまな情報を得ることができますが、それ以外だとあまり期待できません。
ということで、Raspberry Pi 4を用いました。
最近は手に入れやすくなり良かったです。

さらに、同時にACアダプタや配線ケーブルの余りも裏のデッドスペースに収めることにします。
以上の対応で運搬しやすく、またデッドスペースを@<img>{cable}のように活用し、すっきり@<fn>{sukkiri}と収めることができました。
//image[cable][デッドスペースの活用][scale=0.5]
//footnote[sukkiri][すっきりか否かの判断は各個人によって異なります。本番ではちゃんと結束バンド@<emoji>{guitar}で縛ります。]

=== USB Gadget Driver
PCの要件に記載した「USB Gadget Driver」ですが、
何に使うかといえばSquare Terminal専用ハブを不要にするために@<bou>{使いたかった}です。

Square Terminal専用ハブはSquare TerminalとUSB Type-Cで接続され、これを利用するとSquare TerminalからEthernet接続が行なえます@<fn>{sqhub}。
精算機内ではこれを利用してPCとSquare TerminalをEthernetで接続し、PC内でNAPTしiPadのテザリング回線からインターネットに出ていく構成です。
//footnote[sqhub][キャッシュドロワーやレシートプリンターなどの周辺機器接続のためにも使われるようですが、この精算機内では利用していないため割愛します。]

でもType-CでEthernet？それならRaspberry Pi 4でUSB Gadget Driverのg_etherを使えば、Square Terminalと直結できるのでは？という気がしてきます。
が、専用ハブはただのUSB-Ethernetとして動いているわけではないようで、g_etherを使っただけではうまく機能しません。

以前はSquare Stand（第一世代）という製品を使っており、そこでも専用USBハブがありました。
ただ、別に専用でなくても動くんじゃない？と思ってその辺のUSBハブを接続してもうまく反応しない@<fn>{sp1}のと似た空気を感じます。
//footnote[sp1][特に解析などはしていませんが、この辺数行だけ総集編の「COSMIC L0 SP1」に書いていました。お餅の方は探してね！]

Raspberry Pi 4ではUSBProxy@<fn>{usbproxy}のようなUSB Snifferも使えるようなので、今後Square Terminal専用ハブの通信は見てみるつもりです。
なお、Raspberry Pi 4のUSB Gadget Driverは、一般には電源供給用として接続するUSB Type-Cポートを用います。
つまり、この状態で電源供給はGPIOから行う必要がありますが、今回この点もあまり考えておらずこれも今後の課題です。
//footnote[usbproxy][@<href>{https://github.com/nesto-software/USBProxy}]


== 機能増やしたい問題
致命的な問題ではありませんが、いくつか増やしたい機能があったため追加していきます。
主にソフトウェアの改良について述べますが、通信についても検討しており、
今後は検討に検討を重ね結果を踏まえた上で検討を加速していく所存です。

=== ソフトウェア編
ソフトウェアで取り組んだ点は次のとおりです。

: ビルバリ関連
  2000円札への対応と、紙幣の投入枚数制限について取り組みました。
: バーコードリーダー関連
  バーコードの二度読み対策、PayPayオフライン決済に関連した取り組みを行いました。
: Square Terminal関連
  最近追加されたAPIを試しました。

日報みたいになっていますが、詳細は次に示すとおりです。

==== ビルバリ関連
まず、2000円札への対応についてです。
精算機で使っているビルバリは、ハードウェアとしては2000円札に対応しています。
ただ、ソフトウェア側はビルバリの仕様を主にリバースエンジニアリングにより把握して実装しましたが、
2000円札自体が手元になく検証ができない、結果として2000円札は受け入れられない状態になっていました。

できれば対応したいですよね？ということで、まずは検証用2000円札の入手から始めます。
「二千円札@wiki」@<fn>{2kyen}には2000円札取り扱い銀行・支店の情報がまとめられています。
特に関東の場合、「みずほ銀行新宿支店」であれば両替機にも取り扱いがあり行きやすいのではないでしょうか。
//footnote[2kyen][@<href>{https://w.atwiki.jp/2000yen/pages/17.html}]

この情報を参考に2000円札を手に入れ（@<img>{2000yen}）、あとは挙動を確認して実装しました！以上！
//image[2000yen][勝ちまくりモテまくり][scale=0.8]

次に、紙幣の投入枚数制限についてです。
現在使っているビルバリは、投入枚数が1枚までという制限があります。

これは紙幣投入後、万が一誤投入などで返金処理が必要になった場合に現物を返すためにこうした制限が設けられています。
仮に返金処理は考えない、すなわち投入後すぐに収金処理を行えば、続けて紙幣を投入することは可能です。

ただ、収金処理を行ってしまうと万が一返金が必要な場合に現物は返せません。
とはいえ返金が必要な場合は考えにくく、一方で頒布価格的に連続投入が必要な場合も考えにくく…ということで今回は従来どおり「1枚まで」のままとしました。
もし合計金額が1000円を超えたら2000円札を使ってください。

==== バーコードリーダー関連
バーコードリーダー関連は、まず二度読み対策から取り組みました。

このバーコードリーダーはバーコード決済のために利用しており、読み取り後PCを通じ外部の決済センターにバーコードの情報を流します。
実装としては読み取りのたび@<fn>{state}に決済センターに流しており、万が一二度読みが発生すると二重決済となりこれでは@<ruby>{外,インターネット}出た瞬間（加盟店の信用）終わったわです。
実は決済センター側でブロックしてくれるのでは？という可能性はありますが、怖くて検証していません。
//footnote[state][「読み取るべき状態（決済金額が1円以上など）であれば」という条件は付きますが割愛します。]

ということで二度読みは避けなければなりません。
精算機で利用しているバーコードリーダーはDENSO QK30で、これは二度読み防止機能が備わっています。
0.1秒～9.9秒まで100ms間隔で二度読み防止時間を設定できるのですが…試したところたまに二度読みのような挙動となりうまくいきません。

検証に利用していたのはPayPayの支払画面（@<img>{paypay}）で、この画面には同じ内容の1次元バーコードとQRコードが同時に表示されています。
//image[paypay][PayPayバーコード支払い画面][scale=0.4]
一度読み取りに成功したあと、画面を回転させたり移動させると二度読みのような挙動となっていました。
確証は無いですが二度読み禁止時間はバーコード種別によって独立している可能性があり、
現状ではQRコードのみ読み取る設定として1次元バーコードは無視するようにしています。

ただ、PayPayのきせかえデザインによっては待ち受け画面に1次元バーコードしか表示されない場合があります。
この場合は、わざわざ支払画面に飛んでもらう必要があります。
じゃあ1次元バーコードしか読み込まないようにすればいいのでは？というのはそのとおりです。
しかし、せっかくQRコードも読めるリーダーを使っているためできればQRコード側を読みたい。
また、1次元バーコードかQRコードかどちらか早く読み込めた情報を使えば、決済を早く行えるという期待もあります。
そのため、バーコード種別が混在していても二度読みが防止できるようにしたいです。

ここで行った対策は次のとおりです。

: バーコード読み取り後、すぐに読み取り待機状態にする
  QK30では「読み取り可能状態」であればバーコードを読み取ることができ、「読み取り待機（禁止）状態」であればバーコードをかざしても反応しません。
  この設定は上位機器から動的に変更可能であり、何かバーコードを読み取った後すぐに読み取り待機状態とする、数秒後再び読み取り可能状態に遷移するようにしました。
  何度か試しましたが、想定どおりうまく二度読み防止になっているようです。
: 時間的に近く、同じ内容のバーコードは弾く
  上記対応でもおそらく二度読みは防げていると思いますが、念のためもう一つ対策を行います。
  ここでは読み取り時間と内容を記録しておき、近い時間で同じ内容であればエラーにする処理を行いました。
: （おまけ）設定用QRコードの無効化
  これまでの対応でQK30のドキュメントを確認していたら、設定用QRコードの無効化も行うべきと思い実施しました。
  QK30やほとんどの？バーコードリーダーは、バーコードを通じて設定を変えることが可能です。
  万が一、決済用バーコードがこの内容と一致してしまうと問題になるので、無効化しておきました。

次に、PayPayオフライン決済に関連した取り組みを行いました。
PayPayオフライン決済とはCPM方式（ユーザが画面を提示し加盟店側が読み取る方式）の加盟店において、
ユーザ側はオフラインでもバーコードを作成でき、
加盟店側でそのバーコードを読み取りかつ決済センターに送信できれば決済できる機能のことです。

このサークルで利用しているのも決済代行会社経由のCPM方式であり、実際検証したところこの機能が利用できました。
@<img>{offline}はオフライン時にテスト決済し払い戻した記録ですが…これだとオフラインかの区別は付かないですね。
//image[offline][PayPayオフライン決済を試した際のウォレット画面][scale=0.4]

通常の決済とオフライン決済で異なるのは、ユーザの端末が「PayPay♪」と言わないことです。
これまで良くいえば人を信じて、悪く言えば横着してPayPayって聞こえたら大丈夫だろう…と真剣に決済結果の確認を行っていませんでしたが、
こちら側でも確認を行う必要が出てきました。

何らかの決済結果の通知が必要になりますが、QK30では外部からブザーやLEDを制御できます。
自由に制御できるわけではなく数パターンの中から選ぶ必要はありますが、十分です。
もっと言えばスピーカーでWAVファイルも再生できるので本当に「PayPay♪」と喋らせることもできますが…別にいいかなと思います。
これで通知を実現し、一応PayPayオフライン決済にも備えることができました。

==== Square Terminal関連
精算機内のSquare TerminalはTerminal API@<fn>{terminalapi}経由で操作しています。
//footnote[terminalapi][@<href>{https://developer.squareup.com/reference/square/terminal-api}]

いつの間にかTerminal Actionという、外部からSquare Terminalに対し決済以外の操作を行える機能が使えるようになっていました。
ドキュメントを確認しただけですが、次に示す操作が可能です。

: PING
  外部からデバイスがオンラインか、またバッテリー残量などの情報を取得する。
: RECEIPT
  レシートを印刷できる。
  レシートが欲しい場合、これまでは決済のタイミングでのみ印刷できたがタイミングが自由になった。
: CONFIRMATION, SIGNATURE, QR_CODEなど
  確認画面、署名画面、QRコードの表示画面、顧客情報の収集画面、選択ボタン表示画面に切り替えることができる。
  それぞれの画面遷移も設定できる。

ただ、個人的にはあまり使わないかな…という感じです。
QRコードは電子版ダウンロードで使えるかもと思いましたが、いきなりサークル側からこのQRコード読むように言われても困るでしょう。

それよりTerminal APIを使うと待ち受け画面が@<img>{powerdbysquare}のように「Squareのシステムを使用しています」で固定されるんですよね…
変えたいなぁと思ったらこれも変更できるようになっていました。
設定はAPI経由ではなく、Squareダッシュボードからの設定で、画像は4枚までで何秒かごとに切り替えることもできます。
これで@<img>{sqimg}のようにできました。
サンキューSquare。
//subfig[Square Terminalの待ち受け画面]{
//image[powerdbysquare][Powered By Square][scale=0.44]
//image[sqimg][カスタム画像][scale=0.46]
//}

なお、以前から使える決済用のAPIも少し変更されていました。
米国ではカード明細の内容をある程度追記できる機能が追加されたようです。
ただ、日本国内から操作してもハネられるので実用はできません。おわり。

=== 通信編
次に通信編ですが、これはC100のときから変更しておらず「検討した」という内容です。

この精算機にはインターネット接続を必要とします。
Square TerminalのTerminal APIによる操作はインターネット経由でのみ可能で、決済センターへの接続もインターネットが必要なためです。

現在、インターネット回線としてはpovoを契約しiPadでテザリングして確保しています。
povoにトラブルが起きると使えなくなるわけで、次の対策案を検討しました。

: peplink マルチSIMルーターの利用
  複数の回線を束ねてインターネットに接続できる商用製品・サービスです。
  レンタルを行っている会社もあるようですが、安いプランでも3万円/日と現実的ではありません。
: OpenMPTCPRouterの利用
  前述のpeplinkと同様、複数の回線を束ねてインターネットに接続できるOSSです。
  専用ルータとVPSのセットアップが必要で、セットアップ後専用ルータ経由でパケットを流すとVPSに飛び、そのVPSがインターネットの出口となります。
  名前のとおりMPTCPが利用されていますが、UDP/ICMPもカプセル化して転送できるようです。
  ルータはRaspberry Piでも構築でき今回にちょうど良い…と思いましたが、OpenWRTをベースとしているためRaspberry Pi OSの機能が使えなくなること、そして2回線目の確保を考えるうちに現状で十分かな…という気になりボツです。
: 公衆無線LANの利用
  公衆無線LANの利用も考えました。
  セキュリティ的には流れる通信はすでにHTTPSとなっている、もしそれでもリスクがあるようなら一度自宅などにVPN接続すれば済む話です。
  混雑時に繋げられるかは微妙ですが、もし繋がるとすればバックエンドは固定回線であろうことから安定していることが期待できます。
  ただ、認証方式が大抵メールアドレスを入力すると一定時間使える…という方式で、Raspberry Pi内から使うのは手間がかかります。
  d Wi-Fiであれば認証にEAP-PEAPが利用でき、自動接続ができるようですが未検証のため見送りました。
: プライベートLoRaの利用
  LoRAではありません。
  近隣のホテルや会議室からLoRaの電波で中継し…ということも考えましたが、やはり距離的には難しい？それともいける？何も検証しておらず判断できません。
: Starlinkの利用
  サークルスペースでは無理ですが、当日サークル主がStarlinkの設備を担いでトラックヤードなど屋外に出て、サークルスペースとはWi-FiかLoRaかで中継…も考えました。
  でもやっぱり屋外にずっと居たくないのでダメです。

見返すと、OpenMPTCPRouterの利用が現実的だったのではないでしょうか。
今後痛い目を見たら本当に検討します。

なお、OpenMPTCPRouterを利用するとしたら、一度VPSを経由する都合上レイテンシが多少悪化することが予想されます。
Redundant scheduler@<fn>{redundant}も使えるようなので、現状モバイル回線のみのところに固定回線も追加で束ねれば、
固定回線側でレイテンシは改善する可能性はありますが、常に固定回線が利用できるとは限らないのでやはり難しいかも知れません。
//footnote[redundant][複数のパスで同じパケットを冗長に (redundant) 送る。受信側では最も早く到着したパケットのみを利用する。]

とはいえ、レイテンシ悪化は数ms程度と思われ許容範囲でしょう。
バーコード決済の決済センターとの通信が多少@<fn>{keepalive}遅れたところで気付かないと思います。
//footnote[keepalive][なお、KeepAliveやTCP Fast Openは使っていません。1決済ごとにTCP,TLSのHandshakeを行うので全体の遅延としては数msでは済まないと思いますが、どの道気付かないでしょう。]

また、Square Terminalの場合、API経由の操作や決済の際にアメリカの特に西海岸まで行っている雰囲気です。
前段にCloudflareのサーバが挟まりそこはヘッダに``NRT''と書いて返してくれますが、おそらく裏にいるであろうSquareのAPIサーバは``region: us-west-2''と書いて返しています。
そうであればいまさらレイテンシが数ms悪化したところで些細なことです。
状況や決済種別によっても異なる、例えばSuicaの決済ならSquare Terminal自身がTMNのサーバに繋ぐのかも知れませんがよく分かりませんでした！
いかがでしたか？
//footnote[tmn][「Square Terminal」にTMN電子マネー決済サービスの接続を開始（TMN）|ペイメントナビ @<href>{https://paymentnavi.com/paymentnews/104472.html}]


== 前髪すかすか問題
前面パネルを作ります。

パネルといえばアクリルパネルです。
そこにUV印刷でアクセプタンスマークや案内を書けばそれっぽくなるでしょう、ということで外注を考えましたが納期や予算的に難しいものがありました。
また、「カットパスは1つのみ（それ以外は別途見積もり）」という制約もあったりで今回の精算機内では少なくともQK30・ビルバリ・お釣り取り出し口のためにいくつか開口部を開ける必要があり適しません。
さらに今後コインメック、ビルバリ、QK30などの配置を変える可能性もあり、せっかく作ってもすぐに使わなくなると思うとわざわざ外注するのが億劫です。

ただのシールで覆う？でもシール自体も結構高い…と思っていると、FDM式3Dプリンタの造形物に対しレーザープリンターのトナーを転写する方法@<fn>{fdm}が目に入りました。
詳細はURL先の動画で確認したほうが早いですが、文字にして残すと次の手順で転写を行っています。
//footnote[fdm][@<href>{https://www.reddit.com/r/FDM_TonerTransfer/}@<qrcode>{https://www.reddit.com/r/FDM_TonerTransfer/}]

 1. 転写したいテキストや画像の鏡像を作成する
 2. 作成したデータをレーザープリンターでOHPフィルムに印刷する
 3. 3Dプリンタで転写対象の造形を始める
 4. 1層目（または位置合わせに必要な部分）が造形されたら中断する
 5. 3Dプリンタのビルドプレートに2.のOHPフィルムを印刷面がZ軸で上を向くように位置を合わせて重ねる
 6. 4.の造形物を取り除く
 7. OHPフィルムとビルドプレートの間に水を入れ、さらにOHPフィルムを粘着テープで強固に固定する
 8. 最初から造形開始し完了を待つ
 9. 造形物の底面にレーザープリンターのトナーが熱転写される！！！

この情報をもとに試したところ、
1回目（@<img>{onetwo}左）はそれほどうまくいかず、
2回目（@<img>{onetwo}右）は3回見たら死ぬ絵みたいな状況でしたが、
3回目（@<img>{san}）は割りとうまくいきました。
3回目もOHPフィルムが熱で反ったことで若干波打ったりかすれたりはありますが、許容範囲でしょう。
//subfig[熱転写を試した結果]{
//image[onetwo][1回目（左）と2回目（右）][scale=0.7]
//image[san][3回目（精算機全体込み）][scale=0.2]
//}

ここで、1回目は普段の造形どおりとし、2回目は各種パタメータとZ軸を調整…したつもりが正負を間違えて失敗、3回目はZ軸をきちんと調整したという状況です。
3回目のパラメータとしては@<table>{3dprinter}のとおりです。
//tabooular[3dprinter][印刷パラメータ]{
項目                 | 値
====================>|<===============================
素材                 | PLA
初期レイヤー高さ     | 0.1mm（デフォルト0.3mm）
初期レイヤーパターン | 同心円
初期レイヤー速度     | 10mm/s（デフォルト20mm/s）
印刷温度             | 215℃（高め）
Z軸オフセット        | ビルドプレートとノズルを0.1mm離す (+0.1mm)
//}

また、OHPフィルムの固定にはセロテープを使ってしまうと印刷中に剥がれてくる@<fn>{plate}のでマスキングテープと布ガムテープを使って@<img>{heat}のように強固に固定しました。
これでも剥がれて来たのでやりすぎなくらい固定したほうが良かったかも知れません。
//image[heat][OHP固定の様子][scale=0.7]
//footnote[plate][OHPフィルムやビルドプレートの材質にもよると思います。ここで使ったプレートはザラザラした表面を使いましたが、ツルツルの裏面のほうを使ったほうが良かったかも知れません。]

さらに、本当かは検証していませんがレーザープリンターで印刷したてのフィルムのほうが造形物にトナーが定着しやすい、
造形物は十分冷えてから剥がしたほうがトナーが定着しやすいと見たのでそれに従っています。


== まとめと今後の予定
精算機の改良について述べました。
あまり書くこと無いなと思っていましたが、意外とありました。

コインメックやビルバリと制御基板を繋ぐハーネス作りの検討も載せたいところですが、これは実施していないので本当に作ったらまた書きたいと思います。
JSTのコネクタなので純正の圧着加工サービスを頼もうと思いましたが割高、なので互換工具で自作しようと思って材料を揃えたところで放置しています。
わざわざ入手の難しい時期に部品集めたんですが…これが蛙化現象ってやつですか？
そして、そうこうしていたらLCSCがハーネス加工サービスを始めるらしいです。
価格感はいまのところ見積もりを取る必要があり不明ですが、こっちにすれば良かったかもと思っています。

ところで最近は

 1. ヤフオクでジャンク品を買う
 2. 解析する時間が無いのでゴミが溜まる
 3. 部屋が狭くなる
 4. 広い家に引っ越そう！一軒家の注文住宅を建てるんや！
 5. 断熱気密、日射遮蔽取得に始まり内線規定や給排水施工について調べて時間がなくなる
 6. 1.に戻る

という日々を送っております。
本業をおろそかにするとクビになる恐れがあるのでなんとかしないといけない、
そうすると副業がおろそかになり見限られる恐れがあるのでなんとかしないといけない、
そうするとジャンク品を解析できずゴミが溜まるのでなんとかしないといけない。

落ち着いたら精算機の改良も続けたいですが、その前に宝くじで5000兆円当てたい。
おわり。


== おまけ：輸送部活動記録
続きだぜ。

これまでコインメックとビルバリを制御してきました。
あと精算機や券売機、自販機あたりで制御しがいのありそうなのといえば？そう、電子マネー端末です。
なんとなくバーコードリーダーやSquare Terminalで満足していますが、それはそれとして電子マネー端末の制御をしたい。
制御できたところで加盟店登録は無理でしょうがそれはそれとして制御したいです。

ただ、以前から家に居る券売機（@<img>{mamiyaop}）くんは形も古く、仮に電子マネー端末のみを手に入れたとしても@<fn>{sx-20}接続すらできずリバースエンジニアリングのための情報を得られないでしょう。
そこで、電子マネー端末が付いた状態の券売機を追加購入することにします。
結果として、JREM SX-20の付いたグローリーの券売機（@<img>{glory}）と、同じくSX-20の付いたValtecの券売機（@<img>{valtec}）を購入しました。
本章ではこの券売機輸送について紹介します。
//footnote[sx-20][そもそも端末自体手に入らな…と思ったら、ここ数ヶ月ヤフオクでJREM SX-20が出品されていました。なぜ？]
//subfig[券売機くん]{
//image[mamiyaop][Mamiya-OP券売機][scale=0.3]
//image[glory][グローリー券売機][scale=0.3]
//image[valtec][Valtec券売機][scale=0.3]
//}

=== 1台目：Mamiya-OP券売機
参考情報として、以前に購入した券売機の輸送について紹介します。
この券売機は重さ30~40kgほど、大きさも140サイズほどでまぁまぁ小型で、頑張れば一人でも運べます。

このときの販売元はリサイクルショップで、自社便で自宅玄関先まで配送してくれました。
おわり。

=== 2台目：グローリー券売機
今回購入した券売機です。

券売機自体は重さ40kgほど、大きさは120サイズほどとこれも小さめですが、台座込みで出品されています。
この台座まで含めると重さ60~80kg程度、サイズは特に高さが160cmほどになります。

台座まで含んだ状態で発送という条件で、配送方法は西濃運輸のパレット便（営業所止め）か直接引き取りです。
西濃運輸で発送してもらったとしても自宅までの運搬をなんとかする必要があり、西濃運輸の送料も安くはないため直接引き取りを考えます。

こうなると倉庫やガレージを借りたくなってきますが、やはり近場には無く、もしあったとしても基本は屋外となるためそこで解析作業を行うことを考えると厳しいです。
月島倉庫の「ちょ庫っと」というサービスでは、巨大な物流倉庫を小割にして貸し出してくれて荷役の手配やってもらえるそうですが…値段的に都内1Kアパートと同じくらいかかります。
やっぱり自宅に置くことを前提に、直接引き取りで行きましょう。

なお、直接引き取りといっても自分が出向いたわけではなく、ここでは運送業者を手配して運んでもらいました。
もし仮に自分自身で行おうとすると、券売機の高さからして2tトラックを借りて運転する必要がある、また重量的に助手を必要とします。
これは時間も手間もかかる上、そもそもペーパードライバーのため2tトラックの運転自体が厳しく例え法（運転免許）が許しても俺は許しません。

個人が運送業者を手配するといえばこの辺りかと思いますが、あいにく条件が合いませんでした。

: 赤帽
  「軽貨物運送業を営む個人事業主で組織される協同組合」らしいです。RedHatのことではありません。
  使われる車両は軽トラであり、今回の券売機は高さ的に積載不可となる可能性があります。
  車両によっては高さのある荷物でも載せられたり、そもそも券売機を横倒しにすればいけたかも知れませんが横倒して問題が出ないか確認できませんでした。
  重量的にパワーゲートも欲しいですがやはり車両が限られる、また2人以上で運びたいですが基本運転手1名で運ぶ想定のため適しません。
  車両2台に依頼すれば運べるそうですが当然その分費用はかかり、別途便利屋さんに頼んだり自分が出向いて助手になるとすると手間がかかります。
: ヤマト らくらく家財宅急便
  ヤマトホームコンビニエンス（親会社はアート引越センター）が提供する大きな家具家電を運搬できるサービスです。
  今回検討した中で料金は一番安かったですが支払いは発払い限定、つまり自分が直接引き取りを行いその場でらくらく家財宅急便に引き渡し支払いの流れになります。
  まぁ安いし良いか…と思いかけましたが、らくらく家財宅急便が発地に到着する時間は幅があり、長時間発地で待ち続けたり不在にする可能性があり断念しました。
  そもそも、精密品は輸送できないらしく申し込んでも券売機も断られていたかも知れません。
: 引越し業者
  引越し業者の中には重量物運搬のサービスを提供している業者もあり、さらに申込者立会い不要で搬出を行えるサービスもあります。
  ただ、申し込みについての問い合わせで荷物は券売機と伝えたところ断られました。残当。
: PickGoエクスプレス、ハコベル
  荷主と運送業者のマッチングサービスで、赤帽と同じような印象です。
  赤帽と異なるのは2tトラックも選べるという点ですが、基本的に運転手は搬入搬出の作業はしない、作業員追加も不可で適しません。

ラッミス@<fn>{lammis}さんに運んで欲しい。
最終的に、運転手付き2tトラックレンタル+作業員1名追加できる会社に依頼しチャーター便で輸送してもらいました。
検索すると同様のサービスを提供している会社はいくつかありますが、そのうちのどれかです。
//footnote[lammis][ランミスにしか聞こえない。]

金額的には安いとはいえませんが、2tトラックと大人2名の時間を確保して、
行きの分しかお金出していないけど帰りは空荷かも…と考えると、納得というかわからせられます。

デレマスの等身大アクリルスタンド@<fn>{deremasu}も送料33,000円～77,000円となっていますが、
おそらく工場と配送センター・配送センターと着地はチャーター便を使う値段だと思います。
配送センター間は混載輸送ができるかも知れませんが、やはりそのぐらいしますよね。
//footnote[deremasu][@<href>{https://twitter.com/FUNTOS17058649/status/1675414000996323328}]

=== 3台目：Valtec券売機
2台目で十分書いた、と思いましたが3台目もありました。

これはブラウン管のPCモニターくらいのサイズ感で、一人でも持てるくらいです。
ただ、出品者から直接引き取り限定、男性2名以上で来るようにという条件が指定されています。

このときもいろいろ考えましたが、なんやかんやで2台目のときに運搬してもらった業者にお願いしました。
今回は2tトラックではなく軽トラをチャーターしましたが、やはりこれくらいかかるな…という印象です。

とはいえ、お金さえ払えばコネも無い、次いつ依頼するかも分からない個人がWebから申し込み、
確認の電話に「ｱｯﾊｲ」「ｴｯｯｯ」「ﾜｯ」と受け答えすればなんとかなるのは良いですね。
次は改札機とか買って運んでもらいたい。
