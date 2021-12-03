#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep()関数を使う
#include <time.h>


/*
 ファイルによるセルの初期化: 生きているセルの座標が記述されたファイルをもとに2次元配列の状態を初期化する
 fp = NULL のときは、関数内で適宜定められた初期状態に初期化する。関数内初期値はdefault.lif と同じもの
 */
void my_init_cells(const int height, const int width, int cell[height][width], FILE* fp){
    if(fp==NULL){
        /*int first[5][2]={{30,20},{30,22},{31,22},{31,23},{32,20}};
        for(int i=0; i<5; i++){
            cell[first[i][1]][first[i][0]]=1;
        }*/
        int r[10]={1,0,0,0,0,0,0,0,0,0};
        srand((unsigned int)time(NULL));
        for(int y=0; y<height; y++){
            for(int x=0; x<width; x++){
                cell[y][x]=r[rand()%10];
            }
        }
    }else{
        int x,y;
        char life[20];
        char ver[20];
        fscanf(fp, "%s %s", life, ver);       
        while(fscanf(fp, "%d %d",&x, &y) != EOF){           
            cell[y][x]=1;
        }
    }
}

/*
 グリッドの描画: 世代情報とグリッドの配列等を受け取り、ファイルポインタに該当する出力にグリッドを描画する
 */
void my_print_cells(FILE *fp, int gen, const int height, const int width, int cell[height][width]){
    int alive=0;
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            if(cell[y][x]){
                alive++;
            }
        }
    }



    fprintf(fp,"generation = %d, alive: %d, dead: %d\n", gen, alive, height*width-alive); // この場合 (fp = stdout), printfと同じ
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
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            cell[y][x]=newcell[y][x];
        }
    }
}


int main(int argc, char **argv)
{
  FILE *fp = stdout;
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
      //printf("Hello");
      my_init_cells(height,width,cell,lgfile); // ファイルによる初期化
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
  sleep(1); // 1秒休止

  /* 世代を進める*/
  for (int gen = 1 ;; gen++) {
    my_update_cells(height, width, cell); // セルを更新
    my_print_cells(fp, gen, height, width, cell);  // 表示する
    sleep(1); //1秒休止する
    fprintf(fp,"\e[%dA",height+3);//height+3 の分、カーソルを上に戻す(壁2、表示部1)
  }

  return EXIT_SUCCESS;
}
