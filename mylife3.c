#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep()関数を使う
#include <time.h>
#include <math.h>
#include <string.h>


/*
 ファイルによるセルの初期化: 生きているセルの座標が記述されたファイルをもとに2次元配列の状態を初期化する
 fp = NULL のときは、関数内で適宜定められた初期状態に初期化する。関数内初期値はdefault.lif と同じもの
 */
int my_init_cells(const int height, const int width, int cell[height][width], FILE* fp){
    if(fp==NULL){
        /*int first[5][2]={{30,20},{30,22},{31,22},{31,23},{32,20}};
        for(int i=0; i<5; i++){
            cell[first[i][1]][first[i][0]]=1;
        }*/
        int r[10]={1,0,0,0,0,0,0,0,0,0}; //alive:dead = 1:9
        srand((unsigned int)time(NULL));
        for(int y=0; y<height; y++){
            for(int x=0; x<width; x++){
                cell[y][x]=r[rand()%10];
            }
        }
    }else{
        
        int x,y;
        int len=100;   
        char buff[len];
        char next[len];
        next[0]='\0';
        char *token;
        int rle=0;
        int xx;
        int yy;
        
        char num[10];
        int digit;
        int dollcount=0;
        int bcount;
        int ocount;
        int count;

        while(fgets(buff, len, fp)!=NULL){
            if(buff[0]!='#'){//コメント行は読み飛ばす
                if(buff[0]=='x'){//先頭文字がxの場合、これをrleのheader lineと判断する。
                    rle=1;
                    xx = atoi(strtok(buff, " =,x"));
                    yy = atoi(strtok(NULL, " =,y"));
                    //読み込んだファイルの盤面が、mainで指定されているサイズよりも大きかったらエラー
                    if(width<xx || height<yy){
                        fprintf(stderr,"too large\n");
                        return -1;
                    }
                }else{//コメントでもrleのheaderでもない行は、rleか、1.06の盤面情報
                    if(rle){
                        strcpy(buff,strcat(next,buff));//前の行から引き継いだ分をくっつける
                        next[0]='\0';

                        token=strtok(buff, "$!");
                        while(token != NULL){
                            for(int i=0; i<10; i++){
                                num[i]='\0';
                            }
                            count=0;
                            digit=0;
                            if(token[strlen(token)-1]!='\n'){
                                for(int i=0; i<strlen(token); i++){
                                    if(token[i]=='b'){
                                        if(num[0]){
                                            bcount=atoi(num);                        
                                            for(int i=0; i<10; i++){
                                                num[i]='\0';
                                            }
                                            digit=0;
                                        }else{
                                            bcount=1;
                                        }
                                        
                                        count+=bcount;
                                    }else if(token[i]=='o'){
                                        if(num[0]){
                                            ocount=atoi(num);
                                            for(int i=0; i<10; i++){
                                                num[i]='\0';
                                            }
                                            digit=0;
                                        }else{
                                            ocount=1;
                                        }
                                        //現在地からocount個は生きている
                                        for(int j=count; j<count+ocount; j++){
                                            cell[dollcount][j]=1;
                                        }
                                        count+=ocount;
                                    }else if(token[i]==' '){
                                        //スペースは無視
                                    }else{//数字情報を記録
                                        num[digit]=token[i];
                                        digit++;
                                    }
                                }
                                
                                dollcount++;
                            }else{//最後が改行だったとき
                                token[strlen(token)-1]='\0';//改行を消して
                                strcpy(next,token);//次の行に回す
                            }
                            token=strtok(NULL, "$!");
                        }
                    }else{
                        x=atoi(strtok(buff, " "));
                        y=atoi(strtok(NULL, " "));
                        cell[y][x]=1;
                    }
                }               
            }
        }

       
    }

    return 0;
}

/*
 グリッドの描画: 世代情報とグリッドの配列等を受け取り、ファイルポインタに該当する出力にグリッドを描画する
 */
void my_print_cells(FILE *fp, int gen, const int height, const int width, int cell[height][width]){
    //生きている個数を数え上げ
    int alive=0;
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            if(cell[y][x]){
                alive++;
            }
        }
    }



    fprintf(fp,"generation = %5d, alive: %4d, dead: %4d\n", gen, alive, height*width-alive); // この場合 (fp = stdout), printfと同じ
    fprintf(fp,"+");
    for (int x = 0 ; x < width ; x++)
        fprintf(fp, "-");
    fprintf(fp, "+\n");
    
    /* 外壁と 内側のゲーム部分 */
    for (int y = 0; y < height; y++) {
        fprintf(fp,"|");
        for (int x = 0; x < width; x++) {
        // ANSIエスケープコードを用いて、赤い"#" を表示
        // \e[31m で 赤色に変更
        // \e[0m でリセット（リセットしないと以降も赤くなる）
            if(cell[y][x]){                
                fprintf(fp, "\e[31m#\e[0m");               
            }
            else{
                fprintf(fp, " ");
            }
        }
        fprintf(fp,"|\n");
    }

    // 下の壁
    fprintf(fp, "+");
    for (int x = 0 ; x < width ; x++)
        fprintf(fp, "-");
    fprintf(fp, "+\n");
    
    fflush(fp); // バッファされている文字列を出力する
}

/*
 着目するセルの周辺の生きたセルをカウントする関数
 */
int my_count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width]){
    int d[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
    int count=0;
    for(int i=0; i<8; i++){
        if(0<=h+d[i][0] && h+d[i][0]<height && 0<=w+d[i][1] && w+d[i][1]<width){
            if(cell[h+d[i][0]][w+d[i][1]]){
                count++;
            }
        }
    }

    return count;
}

/*
 ライフゲームのルールに基づいて2次元配列の状態を更新する
 */
void my_update_cells(const int height, const int width, int cell[height][width]){
    int newcell[height][width];
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            int around=my_count_adjacent_cells(y,x,height,width,cell);
            if(cell[y][x]){
                if(around==2 || around==3){
                    newcell[y][x]=1;
                }else{
                    newcell[y][x]=0;
                }
            }else{
                if(around==3){
                    newcell[y][x]=1;
                }else{
                    newcell[y][x]=0;
                }
            }
        }
    }
    //一気に更新
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            cell[y][x]=newcell[y][x];
        }
    }
}


int main(int argc, char **argv)
{
  FILE *fp = stdout;
  FILE *output;
  const int height = 40;
  const int width = 70;

  int cell[height][width];
  for(int y = 0 ; y < height ; y++){
    for(int x = 0 ; x < width ; x++){
      cell[y][x] = 0;
    }
  }

  /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
  if ( argc > 2 ) {
    fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
    return EXIT_FAILURE;
  }
  else if (argc == 2) {
    FILE *lgfile;
    if ( (lgfile = fopen(argv[1],"r")) != NULL ) {
      if(my_init_cells(height,width,cell,lgfile)==-1){//ファイルで初期化してみて盤面サイズがおかしかったらエラー
          return EXIT_FAILURE;
      }; // ファイルによる初期化
    }
    else{
      fprintf(stderr,"cannot open file %s\n",argv[1]);
      return EXIT_FAILURE;
    }
    fclose(lgfile);
  }
  else{
    my_init_cells(height, width, cell, NULL); // デフォルトの初期値を使う
  }

  my_print_cells(fp, 0, height, width, cell); // 表示する
  //sleep(1); // 1秒休止
  

  char filename[21]="./output/gen__00.lif"; //outputフォルダに盤面ファイルを吐き出す
  /* 世代を進める*/
  int gen=1;
  while(1){
    my_update_cells(height, width, cell); // セルを更新
    my_print_cells(fp, gen, height, width, cell);  // 表示する
    usleep(0.1*1000*1000); //0.1秒休止する
    if(gen%100==0 && gen<10000){
        filename[12]=gen/1000+'0';
        filename[13]=(gen%1000)/100+'0';

        output = fopen(filename, "w");
        fprintf(output, "#Life 1.06\n");
        for(int x=0; x<width; x++){
            for(int y=0; y<height; y++){
                if(cell[y][x]){
                    fprintf(output,"%d %d\n", x, y);
                }
            }
        }
        fclose(output);
    }
    fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)
    gen++;
  }

  return EXIT_SUCCESS;
}
