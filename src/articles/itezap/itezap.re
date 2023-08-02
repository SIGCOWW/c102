= 私たちの香りって、なに？
@<author>{最近難民キャンプが少なくて寂しい, itezaP;twitter:sugerest}

//profile{
U149お疲れ様でした。
夏はシャルロッテ・ゼンフィスさんが可愛いです。
//}

//lead{
赤城みりあさん、櫻井桃華さん ◎
//}

== ご挨拶

こんにちは、itezaPと申します。
2023春に「アイドルマスター シンデレラガールズ U149」が放送されました。
非常に良かったです（小並感）。
まったく、小学生は（ry。

弊サークルにて、キャラから連想されるタンパク質配列や化合物からキャラの類似度を測る、という記事をいくつか書きました。
今回は再び化合物に注目して、もう少しデータセットを増やして比較した記事になります。
若干ネタ切れ感も否めませんが、よろしくお願いします。

== はじめに

においは、においの元である分子（化合物）が揮発によって空気中に常に放出されています。
人はそれを鼻にあるにおい受容体と結合し、その化学的な刺激によって人はにおいを感じます。
においに関する化学物質は主に有機化合物であり、その種類もさまざまです。
例えばゲラニオールと呼ばれる有機化合物は花の香りの重要な成分として知られています。
においは1種類の分子だけでなく、何十何百と複数のにおい分子が関与します。
人には何百種類ものにおい受容体が存在し、複数のにおい分子との相互作用で最終的に脳はある特定のにおいと特定します。
さらにその分子が含まれる量でも感じ方が変わってきます。
これらが要素が複雑に作用しているため、そのメカニズムはいまだに解明されていない点が多いです。

ある2つのものがどれくらい類似しているかを知ることは重要な課題の1つです。
例えば有機化合物は、類似した構造ならばその性質も類似することが知られています。
化合物の性質とは水溶性や毒性、薬理作用などを指します。
においに関しても同様であり、基本的に類似した構造は類似したにおいであると考えられます。
つまり構造の特徴や類似性に注目すれば、その香りにおいて重要な要素や似たにおいと考えられる化合物を推測できます。

本記事ではバラといちごとゆりの3種類の花の香りに含まれる代表的な分子を対象として、その類似度を簡易的に比較します。
ここでの簡易的としたのは、香りに代表される分子に対象を絞りかつ定性的な比較評価をしているためです。
またメタヒューリスティクスな手法である群知能を使って、分子のフィンガープリントの探索も行ってみました。

== 方法

=== フィンガープリントによる分子類似度の計算

分子を比較する際にはフィンガープリントと呼ばれる手法がよく用いられます。
フィンガープリントとは、ある基準によって決められた部分構造を持つかどうかを複数のビット列で表現したものです。
その部分構造は化学的知見やグラフ理論を基に定義されることが多いです。
化学的知見とはOH基や芳香環を含んでいるか等を指します。
今回利用したMACCSフィンガープリントは、汎用的な化学的知見を基に決められた167のビット列で表現されます。
2つの分子をフィンガープリントで表現することで、どれくらい類似するかを評価することができます。
類似度指標は、分子比較の際によく使われているTanimoto係数を利用します。
Tanimoto係数は以下の式で計算することができます。

//texequation{
Tanimoto(A,B) = \frac{C}{A + B - C}
//}

ここで@<m>{A,B}は各分子フィンガープリントの1の数、@<m>{C}は@<m>{A,B}の同位置に1がある数です。
この数が1に近ければ2つの分子の類似度が高いことを意味します。

=== 群知能による分子フィンガープリントの探索

各分子の類似度や距離が計算できるならば、対象とする2つの分子の中間となるような分子を考えることも可能です。
しかしそのような分子を考えるのは、距離が近そうなビット列すべてを考慮する必要があります。
データの次元数が多い場合、それを一つ一つ人間が考えるのは非常に困難です。
そのような場合、組み合わせ最適化問題を解いて近似解を探索します。
組み合わせ最適化問題を解く場合、特定の問題に依存しない方法としてメタヒューリスティクスが存在します。
メタヒューリスティクスの有名な方法としては、遺伝的アルゴリズム（GA、芸術科アートデザインクラスではない）や群知能が挙げられます。

群知能は鳥やアリといった生物の群れの動きを模して作られたメタヒューリスティクスの一つです。
群知能は組み合わせ最適化問題の解を求めるのに適しています。
解空間上に複数の初期値を設定し、あるルールに則ってそれらを更新して最適な解を探索します。
複数の解候補が同時に解空間を探索するため、最もよい正解に辿り着きやすいメリットがあります。
一方で探索する解候補が多いとその分計算量が多くなったり、同じプログラムでも初期値によって解が異なる場合があるデメリットもあります。
群知能は様々な方法が提案されていますが、今回は粒子群最適化（PSO）を用います。
更新式は以下のように定義されます。

//texequation{
v_{i}^{t+1} = \omega * v_{i}^{t} + \eta_{1} * r * (p_{id} - x_{i}) + \eta_{2} * R * (p_{gd} - x_{i})
//}

//texequation{
x_{i}^{t+1} = x_{i} + v_{i}^{t+1}
//}

ここで@<m>{r, R}はそれぞれ独立した0から1の範囲の乱数、@<m>{p_{id}}は自身の最適解、@<m>{p_{gd}}は粒子全体の最適解です。
@<m>{v_{i}}は移動速度、@<m>{x_{i}}はある粒子ベクトルの1要素となります。
@<m>{\omega, \eta_{1}, \eta_{2}}はハイパーパラメーターで、粒子の動きを調整します。
さらにその粒子が良い解であるかを評価する目的関数を定義する必要があります。
今回は2つの分子とのTanimoto係数がおおよそ0.5付近となる分子の探索を目的とします。
これで2つの分子の中間となるようなフィンガープリントを得ることができます。
目的関数は以下のように定義しました。

//texequation{
f(x) = 0.5 - \frac{Tanimoto(x, fp_{1}) + Tanimoto(x, fp_{2})}{2}
//}

@<m>{x}は粒子ベクトル、@<m>{fp_{1}, fp_{2}}は中間を求めたい分子のフィンガープリントです。
各粒子は上記で定義した更新式で値を更新したのち、目的関数でその解の良さが評価されます。
粒子の位置更新と評価を複数回繰り返すことで、その時点での最も良い解を得ることができます。

=== データセット

使用する香りの対象はバラ、いちご、ゆりの3種類とします。
各香りに含まれる分子は情報が公開されているサイトまたは報告書から引用しました@<fn>{NARO_rose}@<fn>{NARO_lily}@<fn>{straw_1}@<fn>{straw_2}。
上記のバラとゆりについては、香りの傾向でいくつかの分子が分類されています。
今回バラはフルーティ、ゆりはオリエンタルスイートに分類される分子を使用しました。
フルーティは「バラ様の香り＋モモや洋なし、ラズベリーなどのフルーツ様の甘い香り」で、オリエンタルスイートは「甘く華やかでスパイシーなゆりらしい香り」とのことです。
いちごについてはバラやゆりのようにまとまったサイトがなかったため、いくつかの報告書を参考に分子を選択しました。
今回使用した花の種類とそのデータ数を以下の@<table>{dataset}に示します。
また、使用した化合物のPubChemID一覧を示します。
PubChemはアメリカの国立生物工学情報センター（NCBI）によって管理されている化学分子データベースです@<fn>{PUBCHEM}。

//tabooular[dataset][データセット]{
花	|	データ数
==========>|====
バラ	|	10
いちご	|	12
ゆり	|	17
//}

//footnote[NARO_rose][@<href>{https://www.naro.go.jp/laboratory/nivfs/fragrance/rose/components/index.html}]
//footnote[NARO_lily][@<href>{https://www.naro.go.jp/laboratory/nivfs/fragrance/lily/components/index.html}]
//footnote[straw_1][@<href>{https://www.pref.mie.lg.jp/common/content/000399694.pdf}]
//footnote[straw_2][@<href>{https://www.agrinet.pref.tochigi.lg.jp/nousi/seikasyu/seika11/sep_011_3_2.pdf}]
//footnote[PUBCHEM][@<href>{https://pubchem.ncbi.nlm.nih.gov}]

== 結果と考察

=== わたしたちの特徴って、なに？

はじめに香りの類似度を比較しました。
一つのビット列でデータセット中の分子すべてのフィンガープリント情報を表現することで、その香りのフィンガープリントとしました。
それらのフィンガープリントを使ってTanimoto係数を計算することで香りの類似度とします。
結果を@<table>{fregrance_sim}に示します。

//tabooular[fregrance_sim][類似度計算結果]{
比較群	|	類似度
==========>|====
バラ--いちご	|	0.735
バラ--ゆり	|	0.707
いちご--ゆり	|	0.700
//}

結果として、いずれの類似度も0.7程度とそれなりに高くなりました。
ただしバラといちごについては、他の組み合わせより僅かに高くなりました。
各組み合わせについてその香り特有の部分構造を確認した結果、ゆりは多くの特徴的なフラグメントを持っていました。
その一部を挙げると4つの原子で構成される環である4員環や、複数の環が結合した複合環です。
この部分構造がゆり特有の香りを特徴づける要因であることが考えられます。
いちごについては酸素原子Oを含むヘテロ環が特徴的な部分構造でした。
また、バラ特有の部分構造は今回のデータセットからは見つかりませんでした。

次に各香りのデータセットに対して、フィンガープリントで類似度を計算し特徴的な分子を調べました。
データセット中のある分子を残り2つのデータセット分子すべてに対してTanimoto係数を計算し、その平均が最も小さい分子を特徴的な分子としました。
結果としてバラは4-Methoxystyrene、いちごはFuraneol、ゆりはindoleが特徴的な分子となりました。
これらの構造式を@<img>{rep_mols}に示します。
この結果はある程度妥当であると考えられ、Furaneolは酸素原子を含むヘテロ環を含んでいます。
Indoleも六員環と五員環の複合体です。
つまり前実験で確認された特徴的な部分構造を持つ分子が一番類似度が低くなっています。

//image[rep_mols][特徴的な分子][scale=0.7]

== 私たちのこども、ってなに？

前節の実験で香りの特徴的な分子を決めました。
これにより2つの分子との距離が中間となるような分子を考えることができるようになりました。
つまりバラといちごの特徴的な分子を使えば、桃華とありすの子供を考えることができます。
そのような分子を手動で考えるのは大変なので、群知能を使ってそれらしい分子を探索してみます。
今回は粒子数100で100回更新したものを解とします。
各粒子のスタート地点はランダムとします。
ハイパーパラメーターは@<m>{\omega = 0.9, \eta_{1} = 0.5, \eta_{2} = 0.3}としました。

何回か試行して最もよさそうな解のビット列を@<img>{search_mol}に示します。
これはバラ代表分子とのTanimoto係数が0.50、いちご代表分子とのTanimoto係数が0.48となりました。
両者とのTanimoto係数がおおよそ0.5のため、これが中間にある分子の一つといえます。
つまりバラっぽくていちごっぽい香りにおける重要な分子かもしれない、ということです。
実際の香りは複数の分子で構成されているので、香りに含まれる分子群の距離や分布の傾向がわかれば、本格的に香りの構成を考えることができるかもしれません。
それらが解明できれば、桃華とありすの子供の香りが実現できると思います。

//image[search_mol][群知能によって得られた中間分子の一つ][scale=0.7]

今回はフィンガープリントを用いて組み合わせ最適化を解いただけなので、化学的制約を考慮していません。
これは今回探索によって見つけたフィンガープリントと同等の分子が現実に存在するのか、を意味します。
ただしフィンガープリントから化合物を検索するサービスは少し探した限りなかったので、自前で検索するしかないかもしれません。
また特定の性質を持つ化合物を生成する、ということなら深層学習による生成モデルの方が精度や妥当性は高いです。

== おわりに

今回はバラといちご、ゆりの3種類の花を対象として、その香りの代表的な分子を用いた類似度の比較を行いました。
またそれらの香りの代表的な分子を決め、中間となる分子を群知能を用いて探索しました。
定性的な評価なのでこれらの結果の妥当性は低いです。
こういう分野もあるんだな、程度に留めておいていただければと思います。
最後にU149の4話からお気に入りの一枚を掲載して締めとさせていただきます。
（本当はもう少し先のウィンクしたところが一番好きなのですが見つけられませんでした。）

//image[momoka_bestshot][ママ][scale=0.6]
