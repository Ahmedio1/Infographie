#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h> 

typedef struct Surface
{
    int width;  ///largeur
    int height; ///hauteur
    int depth; /// bps
    double *data; ///pixels
}SURFACE;

int iclamp ( int x,int min,int max){
    if (x<min){
        return min;
    }
    if(x>max){
        return max;
    }
    return x;
}

void surface(SURFACE *s,int width,int height){
    s->data = (double *)malloc( width * height * sizeof(double));
    if(s->data !=NULL){
        s->width = width;
        s->height = height;
        s->depth = 8;
    }
}

void _surface(SURFACE *s){
    free(s->data);
}

double *at(SURFACE *s,int x,int y){
    if(x < 0 || x >= s->width) return NULL;
    if(y < 0 || y >= s->height) return NULL;
    return s->data + y * s -> width +x;
    }

void point(SURFACE *s, int x,int y,double value){
    double *pos = at(s,x,s->height-1-y);
    if (pos != NULL)*pos = value;
    }

 void fill(SURFACE *s,double value){
    for (double *i = s-> data, *e=s-> data + s->width * s->height;i!=e;++i){
        *i=value;
    }
 }

 int pgm_write(SURFACE *s, FILE *f){
    int max =1 << s->depth;

    int count = fprintf(f,"P2\n# png_write\n%d %d\n%d\n",s->width,s->height,max-1);

    int cr = s->width;
    for (double *i = s-> data, *e = s->data + s->width * s->height; i!=e;++i){
        count +=fprintf(f,"%d",iclamp(*i *max,0,max-1));
        if(--cr){
            count+=fprintf(f," ");
        }
        else{
            cr=s->width;
            count+=fprintf(f,"\n");
        }
    }

    return count;
 }

int pgm_read(SURFACE *s,FILE *f){
    if (fgetc(f) != 'P')return 0;
    if (fgetc(f) != '2')return 0;
    if (fgetc(f) != '\n')return 0;
    
    char c;
    while((c=fgetc(f))=='#') 
        while (fgetc(f)!='\n');
    ungetc(c,f);

    int width,height,max;
    if(fscanf(f,"%d%d%d",&width,&height,&max)!=3)
    return 0;

    SURFACE surf;
    surface(&surf,width,height);
    if(surf.data == NULL) return 0;

    for (double *i = surf.data, *e =surf.data +surf.width *surf.height; i !=e;++i){

        int value;
        
        if (fscanf(f,"%d",&value)!=1){

            _surface(&surf);
            return 0;
        }

        *i = (double)value /max;
    }

    surf.depth=8;

    _surface(s);

    *s = surf;

    return 1; 

}

void segment(SURFACE *s,int x1,int y1,int x2,int y2){

    int a=(y2-y1)/(x2-x1);
    int b= y1-a*x1;
    for (int x=x1;x<x2;x++){
        int y=floor(a*x+b);
        printf("%d\n",y);
        point(s,x,y,0);
    }
}

void segment_sauf_1er_point(SURFACE *s,int x1,int y1,int x2,int y2){
    int dx,dy;
    dx=x2>x1?x2-x1:x1-x2;
    dy=y2>y1?y2-y1:y1-y2;
    if (dx>dy){
        int a=(y2-y1)/(x2-x1);
        int b= y1-a*x1;
        for (int x=x1+1;x<x2;x++){
            int y=floor(a*x+b);
            point(s,x,y,0);
    }
    }
    else{
        int a=(y2-y1)/(x2-x1);
        int b=y1-a*x1;
        if(y1<y2){
        for (int y=y1+1;y<=y2;y++){
            int x=floor((y-b)/a);
            //printf("%d\n",(y-b)/a);
            point(s,x,y,0);
        }
        }
        else{
            for (int y=y1+1;y>=y2;y--){
            int x=floor((y-b)/a);
            //printf("%d\n",(y-b)/a);
            point(s,x,y,0);
        }
        }
    } 
    }

void triangle(SURFACE *s,int x1,int y1,int x2,int y2,int x3,int y3){
    segment_sauf_1er_point(s,x1,y1,x2,y2);
    segment_sauf_1er_point(s,x2,y2,x3,y3);
    segment_sauf_1er_point(s,x3,y3,x1,y1);
}

void segement_gris(SURFACE *s,int x1,int y1,double g1 ,int x2,int y2,double g2){
 int dx,dy;
 double g=g1;
    dx=x2>x1?x2-x1:x1-x2;
    dy=y2>y1?y2-y1:y1-y2;
    if (dx>dy){
        int a=(y2-y1)/(x2-x1);
        int b= y1-a*x1;
        for (int x=x1+1;x<x2;x++){
            int y=floor(a*x+b);
            g+=1/(x2-x1);
            point(s,x,y,g);
    }
    }
    else{
        int a=(y2-y1)/(x2-x1);
        int b=y1-a*x1;
        if(y1<y2){
        for (int y=y1+1;y<=y2;y++){
            int x=floor((y-b)/a);
            g+=(double)1/(y2-y1);
            printf("%f\n",g);
            point(s,x,y,g);
        }
        }
        else{
            for (int y=y1+1,g=g1;y>=y2;y--,g+=0.1){
            int x=floor((y-b)/a);
            g+=(double)1/(y2-y1);
            point(s,x,y,g);
            }
        }
    } 
}

void tracertriangle(SURFACE *s,int x1,int y1,int x2,int y2,int x3,int y3){
    int xi;
    double a=(x1-x3)/(y1-y3);
    int b=y3-a*x3;
    xi=(y3-b)/a;
    printf("%d",xi);
    //triangle(s,x2,y2,xi,y2,x3,y3);

}


void translation(SURFACE *s,int x,int y,int x1,int y1,int valx,int valy){
    int a=(y1-y)/(x1-x);
    int b= y1-a*x1;
    for (int i=x;i<x1;i++){
        int j=floor(a*i+b);
        printf("%d\n",y);
        point(s,i+valx,j+valy,0);
}
}

void cisaillement(SURFACE *s,int x1,int y1,int x2,int y2){
    int dx,dy;
    dx=x2>x1?x2-x1:x1-x2;
    dy=y2>y1?y2-y1:y1-y2;
    if (dx>dy){
        int a=(y2-y1)/(x2-x1);
        int b= y1-a*x1;
        int x2p=x2*a+y2;
        for (int x=x1;x<x2p;x++){
            int y=floor(a*x+b);
            point(s,x,y,0);
    }
    }
    else{
        int a=(y2-y1)/(x2-x1);
        int b=y1-a*x1;
        int x2p=x2*a+y2;
        if(y1<y2){
        for (int y=y1+1;y<=y2;y++){
            int x=floor((y-b)/a)*a+y2;
            point(s,x,y,0);
        }
        }
        else{
            for (int y=y1+1;y>=y2;y--){
            int x=floor((y-b)/a)*a+y2;
            point(s,x,y,0);
            }
        }
    } 
}




//function calcul multiplication de matrice
void mult_matrice(double *A,double *B,double *C,int n,int m,int p){
    for (int i=0;i<n;i++){
        for (int j=0;j<p;j++){
            C[i*p+j]=0;
            for (int k=0;k<m;k++){
                C[i*p+j]+=A[i*m+k]*B[k*p+j];
            }
        }
    }
}

int main(){

    SURFACE surf={0};

    //FILE *input = fopen("chat.pgm","r");
    
   //if(input==NULL){

        surface(&surf,1000,1000);
        assert(surf.data!=NULL);
        fill(&surf,1.);
        //printf("HELLO");
        //triangle(&surf,100,100,900,100,500,900);        
        //segement_gris(&surf,5,20,0,700,900,1);
        //tracertriangle(&surf,600,100,300,400,600,800);
        segment(&surf,5,20,700,900);
        //translation(&surf,5,20,700,900,50,50);
        //segment_sauf_1er_point(&surf,250,20,251,500);
        //cisaillement(&surf,250,20,251,500);
      
    //}
    /*else{
        int ok = pgm_read(&surf,input);
        fclose(input);
        assert(ok);
        assert(surf.data!=NULL);

    }*/
    /*for(int i=0;i<=surf.width;i++){
        for(int j=0;j<=surf.height;j++){
            double*pixel=at(&surf,i,j);
            if(pixel!=NULL){
            point(&surf,i,j,0.255); 
            }
        }
    }*/



    
    FILE *output = fopen("test.pgm","w");
    assert(output != NULL);
    
    pgm_write(&surf,output);
    
    fclose(output);

    _surface(&surf);

    return 0;

}