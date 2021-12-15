#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // sleep()関数を使う
#include <time.h>
#include <math.h>
#include <string.h>

int sum(int n, int list[n]){
    int s=0;
    for(int i=0; i<n; i++){
        s+=list[i];
    }
    return s;
}
int max(int n, int list[n]){
    int m=0;
    for(int i=0; i<n; i++){
        if(list[i]>=m){
            m=list[i];
        }
    }
    return m;
}
/*
 ファイルによるセルの初期化: 生きているセルの座標が記述されたファイルをもとに2次元配列の状態を初期化する
 fp = NULL のときは、関数内で適宜定められた初期状態に初期化する。関数内初期値はdefault.lif と同じもの
 */
int my_init_cells(const int height, const int width, int cell[height][width], FILE* fp, int *n_species){
    if(fp==NULL){
        
        int r[10]={1,0,0,0,0,0,0,0,0,0}; //alive:dead = 1:9
        int species[*n_species];
        for(int i=0; i<*n_species; i++){
            species[i]=i+1;
        }
        srand((unsigned int)time(NULL));
        int tmp;
        for(int y=0; y<height; y++){
            for(int x=0; x<width; x++){
                tmp = r[rand()%10];
                if(tmp){
                    cell[y][x]=species[rand()%*n_species];
                }else{
                    cell[y][x]=0;
                }
                
            }
        }
    }else{
        
        int x,y;
        char *n;
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
                }else if(buff[0]=='n'){
                    *n_species=atoi(strtok(buff, " =n"));
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
                        n=strtok(NULL, " ");
                        if(n!=NULL){
                            cell[y][x]=atoi(n);
                        }else{
                            cell[y][x]=1;
                        }
                        
                        
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
void my_print_cells(FILE *fp, int gen, const int height, const int width, int cell[height][width], char rule[], int n_species){
    //生きている個数を数え上げ
    int alive[n_species];
    char color[4]={'R', 'G', 'Y', 'B'};
    
    for(int i=0; i<n_species; i++){
        alive[i]=0;
    }
    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            if(cell[y][x]){
                alive[cell[y][x]-1]++;
            }
        }
    }
    int alive_ratio[n_species];
    for(int i=0; i<n_species; i++){
        alive_ratio[i]=round((double)alive[i]/(height*width)*(width+2));
    }


    fprintf(fp,"rule: %s, gen = %5d, ", rule, gen); // この場合 (fp = stdout), printfと同じ
    for(int i=0; i<n_species; i++){
        fprintf(fp,"%c: %4d, ", color[i], alive[i]);
    }
    
    fprintf(fp,"dead: %4d\n", height*width-sum(n_species, alive));

    for(int i=0; i<n_species; i++){
        for(int j=0; j<alive_ratio[i]; j++){
            fprintf(fp, "\e[%dm#\e[0m", 31+i);
        }
    }
    for(int i=0; i<width+2-sum(n_species,alive_ratio); i++){
        fprintf(fp,"#");
    }
    fprintf(fp,"\n");


    fprintf(fp,"+");
    for (int x = 0 ; x < width ; x++)
        fprintf(fp, "-");
    fprintf(fp, "+\n");
    
    /* 外壁と 内側のゲーム部分 */
    for (int y = 0; y < height; y++) {
        fprintf(fp,"|");
        for (int x = 0; x < width; x++) {
        // ANSIエスケープコードを用いて、"#" を表示
        // 種類に応じて色分け
        // \e[0m でリセット（リセットしないと以降も赤くなる）
            if(cell[y][x]){  
                if(cell[y][x]==1){              
                    fprintf(fp, "\e[31m#\e[0m"); 
                }else if(cell[y][x]==2){
                    fprintf(fp, "\e[32m#\e[0m"); 
                }else if(cell[y][x]==3){
                    fprintf(fp, "\e[33m#\e[0m"); 
                }else if(cell[y][x]==4){
                    fprintf(fp, "\e[34m#\e[0m"); 
                }             
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
void my_count_adjacent_cells(int h, int w, const int height, const int width, int cell[height][width], int n_species, int n_count[n_species]){
    int d[8][2] = {{-1,-1},{-1,0},{-1,1},{0,-1},{0,1},{1,-1},{1,0},{1,1}};
    for(int i=0; i<n_species; i++){
        n_count[i]=0;
    }
    
    for(int i=0; i<8; i++){
        if(0<=h+d[i][0] && h+d[i][0]<height && 0<=w+d[i][1] && w+d[i][1]<width){
            if(cell[h+d[i][0]][w+d[i][1]]){
                n_count[cell[h+d[i][0]][w+d[i][1]]-1]++;
            }
        }
    }
}

void make_rule(char rule[], int born[], int survive[]){
    char _born[15];
    char _survive[15];

    strcpy(_born,strtok(rule,"/"));
    strcpy(_survive,strtok(NULL,"/"));

    for(int i=1; i<strlen(_born); i++){
        born[i-1]=_born[i]-'0';
    }
    for(int i=1; i<strlen(_survive); i++){
        survive[i-1]=_survive[i]-'0';
    }
}



/*
 ライフゲームのルールに基づいて2次元配列の状態を更新する
 */
void my_update_cells(const int height, const int width, int cell[height][width], int born[10], int survive[10], int n_species){
    int newcell[height][width];
    int n_around[n_species];

    for(int y=0; y<height; y++){
        for(int x=0; x<width; x++){
            my_count_adjacent_cells(y,x,height,width,cell,n_species,n_around);
            int bcheck=0;
            int scheck=0;
            
            if(cell[y][x]){
                for(int i=0; i<10; i++){
                    if(survive[i] != -1){
                        if(sum(n_species,n_around)==survive[i]){
                            scheck=1;
                            break;
                        }
                    }
                }
                if(scheck){
                    newcell[y][x]=cell[y][x];
                }else{
                    newcell[y][x]=0;
                }
                
            }else{
                for(int i=0; i<10; i++){
                    if(born[i] != -1){
                        if(sum(n_species,n_around)==born[i]){
                            bcheck=1;
                            break;
                        }
                    }
                }
                if(bcheck){
                    int m=max(n_species,n_around);
                    int new_i=rand()%n_species;
                    while(n_around[new_i]!=m){
                        new_i=rand()%n_species;
                    }
                    newcell[y][x]=new_i+1;
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
  char Rule[30];
  int n_species=1;
  int cell[height][width];
  for(int y = 0 ; y < height ; y++){
    for(int x = 0 ; x < width ; x++){
      cell[y][x] = 0;
    }
  }

  /* ファイルを引数にとるか、ない場合はデフォルトの初期値を使う */
  if ( argc > 3 ) {
    fprintf(stderr, "usage: %s [filename for init]\n", argv[0]);
    return EXIT_FAILURE;
  }
  else if (argc == 3) {//2個ならルール盤面ファイルの順で
    strcpy(Rule,argv[1]);
    FILE *lgfile;
    if ( (lgfile = fopen(argv[2],"r")) != NULL ) {
      if(my_init_cells(height,width,cell,lgfile,&n_species)==-1){//ファイルで初期化してみて盤面サイズがおかしかったらエラー
          return EXIT_FAILURE;
      }; // ファイルによる初期化
    }
    else{
        if(atoi(argv[2])){
            n_species=atoi(argv[2]);
            if(1<=n_species && n_species<=4){
                my_init_cells(height,width,cell,NULL,&n_species);
            }else{
                fprintf(stderr,"too large number of species: %d\n",n_species);
                return EXIT_FAILURE;
            }
        }else{
            fprintf(stderr,"cannot open file %s\n",argv[2]);
            return EXIT_FAILURE;
        }
    }
    fclose(lgfile);
  }
  else if(argc==2){//1個の場合は、ルールの指定のみで初期盤面はランダム。種数は1。
      my_init_cells(height,width,cell,NULL,&n_species);
      int check=1;
      for(int i=0; i<strlen(argv[1]); i++){
          if(argv[1][i]=='.'){
              check=0;
              break;
          }
      }
      if(check){
        strcpy(Rule,argv[1]);
      }else{
          fprintf(stderr,"You didn't define rule.\n");
            return EXIT_FAILURE;
      }
  }
  else{//引数なしは、初期盤面ランダムでルールはConway's life。種数は1。
    my_init_cells(height, width, cell, NULL,&n_species); // デフォルトの初期値を使う
    strcpy(Rule,"B3/S23");
  }
    
    int born[10];
    int survive[10];
    for(int i=0; i<10; i++){
        born[i]=-1;
        survive[i]=-1;
    }

    char c_rule[30];
    strcpy(c_rule,Rule);

    make_rule(c_rule, born,survive);

  my_print_cells(fp, 0, height, width, cell, Rule, n_species); // 表示する
  
  char filename[21]="./output/gen__00.lif"; //outputフォルダに盤面ファイルを吐き出す
  /* 世代を進める*/
  int gen=1;
  while(1){
    my_update_cells(height, width, cell, born, survive, n_species); // セルを更新
    my_print_cells(fp, gen, height, width, cell, Rule, n_species);  // 表示する
    usleep(0.1*1000*1000); //0.1秒休止する
    if(gen%100==0 && gen<10000){
        filename[12]=gen/1000+'0';
        filename[13]=(gen%1000)/100+'0';

        output = fopen(filename, "w");
        fprintf(output, "#Life 1.06\n");
        if(n_species!=1){
            fprintf(output, "n = %d\n", n_species);
        }
        
        for(int x=0; x<width; x++){
            for(int y=0; y<height; y++){
                if(cell[y][x]){
                    if(n_species==1){
                        fprintf(output,"%d %d\n", x, y);
                    }else{
                        fprintf(output,"%d %d %d\n", x, y, cell[y][x]);
                    }
                }
            }
        }
        fclose(output);
    }
    fprintf(fp,"\e[%dA",height+4);//height+4 の分、カーソルを上に戻す(壁2、表示部2)
    gen++;
  }

  return EXIT_SUCCESS;
}
