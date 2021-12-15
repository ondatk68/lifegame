# lifegame
## 基本課題補足
* 出力ファイルを格納するフォルダを作るために実行ファイルがあるディレクトリで`mkdir output`してください。
* `usleep()`を使って更新を0.1秒刻みにした。  
* rleに関しては、左上を揃えて、ファイルのxyサイズが`main()`のwidth, heightより大きい場合にはエラーになるようにした。  
* 一文字目がxである行があるかどうかで、rleか1.06かを判断した。

## 発展課題
コマンドライン引数でルール及び初期盤面を指定。  
ルールの書式はB/S notationに則る。` B{number list}/S{number list}`  
[ルール一覧](https://www.conwaylife.com/wiki/List_of_Life-like_cellular_automata)  
 
引数を2つ指定する場合は ルール→初期盤面の順で指定する。1.06形式を拡張した、生物種情報を含むファイル(後述)を指定することも可能。
```bash
./mylifegame4 B3/S23 gosperglidergun.lif
```
あるいは、  
初期盤面はランダムで種数のみ指定することも可能。  
種数は1~4の数字を指定できる。(5以上だとエラーになる)  
```bash
./mylifegame4 B3/S23 4
```

引数が1つの場合はルールのみ指定できる(ファイルの指定は不可)。初期盤面はランダムで種数は1になる。
```bash
./mylifegame4 B3/S23
```
引数がなしの場合はルールはB3/S23(Conway's Life)で盤面はランダムになる。種数は1になる。
```bash
./mylifegame4
```
### 複数種の時の更新のルール
* そのマスにいる時  
周りの生き物の種類にかかわらず、合計の数が合えば生き残る。  
* いない時  
周りの生き物の合計の数があっていたら、その中で一番多い種類の生き物が生まれる。一番多い生き物が複数種あった場合は、その中からランダムで選ばれる。
### 表示について
ルール、世代、各色の数(Red,Green,Blue,Yellow)、そして各色の大まかな比率を帯グラフで示している。
### 生物種情報を含む拡張版1.06について
* 1.06の形式を拡張して、`x y n`とすることでその座標にいる生物種を表現するようにした。    
* また、最初の行で`n = 4`などのように種数を明示する。    
* 種数が1の時はnは省略可能。出力ファイルも省略した形になる。  
* この形式のファイルを指定して初期盤面とすることも可能。
* 例として`example.lif`を同封してある。