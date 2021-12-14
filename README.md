# lifegame
## 基本課題補足
* 焦ったいので、`usleep()`を使って更新を0.1秒刻みにしました。  
* rleに関しては、左上を揃えて、ファイルのxyサイズが`main()`のwidth, heightより大きい場合にはエラーになるようにしました。  
* 一文字目がxである行があるかどうかで、rleか1.06かを判断しています。

## 発展課題
コマンドライン引数でルール及び初期盤面を指定  
ルールの書式はB/S notationに則る。` B{number list}/S{number list}`  
[ルール一覧](https://www.conwaylife.com/wiki/List_of_Life-like_cellular_automata)  
 
引数を2つ指定する場合は必ず ルール→初期盤面の順で指定する。  
```bash
./mylifegame4 B3/S23 gosperglidergun.lif
```
引数が1つの場合はルールのみ指定できる。初期盤面はランダムになる。
```bash
./mylifegame4 B3/S23
```
引数がなしの場合はルールはB3/S23(Conway's Life)で盤面はランダムになる。つまり、2つ目の例と同じ挙動を示す。
```bash
./mylifegame4
```