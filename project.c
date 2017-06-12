#include<stdio.h>
char stage[5][30][30]={};

void map_test(int i)//지도의 $과 O의 갯수 확인 - 죵료 시키기 적용해야함!
{
  int sum1 = 0, sum2 = 0, j, k;

  for(j = 0 ; j < 30 ; j++)
  {
  for(k = 0 ; k < 30 ; k++)
  {
    if(stage[i][j][k] == 'O')
    sum1++;
    else if(stage[i][j][k] == '$')
    sum2++;
  }
}
if( sum1 != sum2)
{
  printf("%d번째 맵은 잘못된 map입니다! 종료합니다", i+1);
}

}

void map()//맵 불러오기
{
FILE *map;
int i = 0, j = 0, k = 0;
char a;
map = fopen("map.txt", "r");

while(fscanf(map,"%c",&a) != EOF)
{
if(a == 'm')
continue;
if(a == 'a')
continue;
if(a == 'p')
continue;
if(a == 'e')
  {
    i++;
  j = 0;
  k = 0;
  if(i >= 5)
  {
    break;
  }
  continue;
}
if(a == 'n')
break;//문자 제거
    stage[i][j][k] = a;
  k++;
  if(k >= 30)
  {
  k = 0;
  j++;
  }

  if(j >= 30)
  {
  j = 0;
  i++;
  }

  if(i >= 5)
  break;
  }

  fclose(map);

 //for( i = 0 ; i < 5 ; i++)
 {
   map_test(i);
 for(j = 0 ; j < 30 ; j++)
 {
 for(k = 0 ; k < 30 ; k++)
 {
 printf("%c", stage[1][j][k]);
}
 }
}
}

 int main(void)
 {
 map();//함수내에 i값을 조정하면 맵별로 불러집니다.

return 0;
}
