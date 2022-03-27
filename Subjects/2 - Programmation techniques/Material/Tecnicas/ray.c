#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ppm.c"

typedef struct esfera {
    float r;
    float g;
    float b;
    float x;
    float y;
    float z;
    float raio;
} Esfera;

typedef struct coord {
    float x;
    float y;
    float z;
} Coord;

float intersecao (Esfera c, Coord p, Coord v);
Coord vetorLuz (Coord l, Coord i);
float moduloVetor (Coord p);
float Cos (Coord p, Coord i);

int main () {
    char nome[20];
    Esfera *c;
    int x, y, a, b, i, n=1, flag,  largura, altura, ml, ma;
    float kd = 0.7, ka = 0.3, t, fator, normfactor, dist, distaux;
    // Criei uma struct com x, y, z j� que
    // s�o utilizadas v�rias coordenadas
    Coord cam, l, p, v, it, L, N, d;
    // Outra struct para armazenar os valores r, g, b
    Cor preto, col;
    // Inicializa a cor "preta"
    preto.red = 0;
    preto.green = 0;
    preto.blue = 0;
    printf("Digite a largura: ");
    scanf("%d",&largura);
    printf("Digite a altura: ");
    scanf("%d",&altura);
    // Cria a imagem de acordo com a biblioteca ppm
    Imagem teste;
    teste = novaImagem(largura, altura);
    // Metade da altura e largura para uso posterior
    ma = (int)(altura/2);
    ml = (int)(largura/2);
    // Coordenadas da camera
    cam.x = 0;
    cam.y = 0;
    cam.z = -(ma);
    // Coordenadas da luz
    l.x = 0;
    l.y = altura;
    l.z = 0;
    // L� a quantidade de esferas e cria um vetor
    printf("Digite o numero de esferas: ");
    scanf("%d",&n);
    c = (Esfera*) malloc(sizeof(Esfera)*n);
    // Leitura dos atributos de cada esfera
    for (i = 0; i < n; i++) {
        printf("Esfera %d: \n",i);
        printf("x: ");
        scanf("%f",&c[i].x);
        printf("y: ");
        scanf("%f",&c[i].y);
        printf("z: ");
        scanf("%f",&c[i].z);
        printf("raio: ");
        scanf("%f",&c[i].raio);
        printf("r: ");
        scanf("%f",&c[i].r);
        printf("g: ");
        scanf("%f",&c[i].g);
        printf("b: ");
        scanf("%f",&c[i].b);
    }
    // Ponto z do plano (imagem) ser� sempre 0
    p.z = 0;
    // "a" e "b" s�o contadores para varrer cada pixel da imagem, de cima
    // para baixo, utilizados para "pintar" o pixel
    a = 0;
    // x e y s�o as coordenadas de cada pixel
    for (y = ma; y > -ma; y--, a++) {
        b = 0;
        for (x = -ml; x < ml; x++, b++) {
            // Pinta o pixel de preto inicialmente
            teste->I[a][b] = preto;
            p.x = x;
            p.y = y;
            // Come�o dos calculos, igual do PDF (usei v em vez de d n�o sei porque)
            v.x = cam.x - p.x;
            v.y = cam.y - p.y;
            v.z = cam.z - p.z;
            distaux=0;
            flag=0;
            // Fazemos os c�lculos para cada uma das esferas
            for (i = 0; i < n; i++) {
                // Verifica a exist�ncia de interse��o entre o raio e
                // a esfera (delta), verificar fun��o
                t = intersecao(c[i], p, v);
                // Caso n�o haja interse��o, pula para pr�xima esfera
                if (t<0) continue;
                // Ponto de interse��o
                it.x = p.x + t * v.x;
                it.y = p.y + t * v.y;
                it.z = p.z + t * v.z;
                // Vetor normal
                N.x = (it.x - c[i].x)/c[i].raio;
                N.y = (it.y - c[i].y)/c[i].raio;
                N.z = (it.z - c[i].z)/c[i].raio;
                // Vetor da luz, etc
                L = vetorLuz(l, it);
                normfactor = moduloVetor(L);
                L.x/=normfactor;
                L.y/=normfactor;
                L.z/=normfactor;
                // Calcula o fator, ver fun��o
                fator = Cos(L,N);
                if (fator<0) fator = 0;
                // Define a cor final do pixel de acordo com o fator
                col.red = kd * fator * c[i].r + ka * c[i].r;
                col.green = kd * fator * c[i].g + ka * c[i].g;
                col.blue = kd * fator * c[i].b + ka * c[i].b;
                // Vetor da camera at� o pixel
                d.x = cam.x - it.x;
                d.y = cam.y - it.y;
                d.z = cam.z - it.z;
                // Dist�ncia do raio (caso seja encontrado mais de uma esfera
                // usamos a dist�ncia para verificar qual est� na frente
                dist = moduloVetor(d);
                if (t>=0) {
                    if (dist<distaux || flag ==0) {
                        // "Pinta" o pixel
                        teste->I[a][b] = col;
                        distaux=dist;
                        flag=1;
                    }
                }
            }
        }
    }
    // Salva o arquivo e destroi a imagem da mem�ria
    // de acordo com a biblioteca ppm
    printf("Digite um nome para salvar (ex img1.ppm): ");
    scanf("%s",&nome);
    printf("\nSalvando!\n");
    salvaImagem(nome, teste);
    destroiImagem(teste);
    return 0;
}

float intersecao (Esfera c, Coord p, Coord v) {
    float A, B, C, D, t, t1, t2;
    Coord it;
    A = (v.x * v.x + v.y * v.y + v.z * v.z);
    B = 2 * v.x * (p.x - c.x) + 2 * v.y * (p.y - c.y) + 2 * p.z * (p.z - c.z);
    C = c.x * c.x + c.y * c.y + c.z * c.z + p.x * p.x + p.y * p.y + p.z * p.z -
        2 * (c.x * p.x + c.y * p.y + c.z * p.z) - c.raio * c.raio;
    D = B * B - 4 * A * C;
    t = -1;
    if (D>=0) {
        t1 = (-B - sqrt(D))/(2*A);
        t2 = (-B + sqrt(D))/(2*A);
        t = (t1>t2)?t1:t2;
    }
    return t;
}

float moduloVetor (Coord p) {
    return sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
}

float Cos (Coord p, Coord i) {
    return (p.x * i.x + p.y * i.y + p.z * i.z)/(moduloVetor(p))*(moduloVetor(i));
}

Coord vetorLuz (Coord l, Coord i) {
    Coord L;
    L.x = l.x-i.x;
    L.y = l.y-i.y;
    L.z = l.z-i.z;
    return L;
}
