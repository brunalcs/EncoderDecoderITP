#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

int calculo_digito_verificador(int digito_id[7]) {
    int soma = 0;
    int i;

    for (i = 0; i < 7; i++) {
        if (i % 2 == 0) {
            soma = soma + digito_id[i] * 3;
        } else {
            soma = soma + digito_id[i] * 1;
        }
    }

    int proximo_multiplo = ((soma + 9) / 10) * 10;

    return proximo_multiplo - soma;
}

bool validar_digito_id(int digito_id[8]) {
    int calculo_digito = calculo_digito_verificador(digito_id);

    return calculo_digito == digito_id[7];
}

// Fun��o para converter os 4 primeiros números usando a tabela LCode
void converter_LCode(int numero, char binario[8]) {
    const char *tabela_binaria[10] = {
        "0001101", // 0
        "0011001", // 1
        "0010011", // 2
        "0111101", // 3
        "0100011", // 4
        "0110001", // 5
        "0101111", // 6
        "0111011", // 7
        "0110111", // 8
        "0001011"  // 9
    };

    strcpy(binario, tabela_binaria[numero]); // Copia a string da tabela
}

// Função para converter os 4 últimos números usando a tabela RCode
void converter_RCode(int numero, char binario[8]) {
    const char *tabela_binaria[10] = {
        "1110010", // 0
        "1100110", // 1
        "1101100", // 2
        "1000010", // 3
        "1011100", // 4
        "1001110", // 5
        "1010000", // 6
        "1000100", // 7
        "1001000", // 8
        "1110100"  // 9
    };

    strcpy(binario, tabela_binaria[numero]); // Copia a string da tabela
}

//função para gerar a sequencia do código de barras
void sequencia_ean8(int digito_id[8], char *sequencia){
    int i;

    //adiciona o marcador inicial
    strcat(sequencia, "101");

    //adicionando os 4 primeiros dígitos em LCode para a string sequencia
    for (i=0;i<4;i++){
        char numeros_binarios[8];
        //convertendo os dígitos para LCode e add em numeros_binarios
        converter_LCode(digito_id[i], numeros_binarios);
        //add de numeros_binarios para a string sequencia
        strcat(sequencia, numeros_binarios);
    }

    //marcador central
    strcat(sequencia, "01010");

    //adicionando os 4 ultimos dígitos em RCode para a string sequencia
    for(i=0;i<4;i++){
        char numeros_binarios[8];
        converter_RCode(digito_id[i+4], numeros_binarios);
        strcat(sequencia, numeros_binarios);
    }

    //marcador final
    strcat(sequencia, "101");

}

//função para gerar PBM
void gerar_pbm(const char *sequencia, int largura_area, int altura, int espaco_lateral, const char *nome_arquivo){

    //calculo para a largura geral da img
    int largura_codigo = strlen(sequencia) * largura_area;
    //soma +2 pois o espaçamento é dos dois lados
    int largura_total = largura_codigo + 2 * espaco_lateral; 
    int altura_total = altura; 

    FILE *arquivo = fopen(nome_arquivo, "w");
    if(!arquivo){
        printf("Erro ao abrir o arquivo '%s' para escrita\n", nome_arquivo);
        return; 
    }

    fprintf(arquivo, "P1\n");
    fprintf(arquivo, "%d %d\n", largura_total, altura_total);

    // Gerar a imagem pixel por pixel
    for (int y = 0; y < altura_total; y++) {
        for (int x = 0; x < largura_total; x++) {
            if (x < espaco_lateral || x >= largura_total - espaco_lateral) {
                // Espaçamento lateral (pixels brancos)
                fprintf(arquivo, "0 ");
            } else {
                // Determinar a posição na sequência
                int posicao_codigo = (x - espaco_lateral) / largura_area;
                if (sequencia[posicao_codigo] == '1') {
                    fprintf(arquivo, "1 "); // Barra preta
                } else {
                    fprintf(arquivo, "0 "); // Espaço branco
                }
            }
        }
        fprintf(arquivo, "\n"); // Nova linha após cada linha de altura
    }

    fclose(arquivo);
    printf("Arquivo PBM\n", nome_arquivo);



}

int main(int argc, char *argv[]) {
    if (argc < 2 || argc > 6) {
        printf("Uso: %s <8-digitos> [espacamento_lateral] [pixels_por_area] [altura] [nome_da_imagem.pbm]\n", argv[0]);
        return 1;
    }

    char *entrada = argv[1];
    int digito_id[8];
    char LCode[4][8]; // Tamanho ajustado para armazenar 7 bits + '\0'
    char RCode[4][8]; // Tamanho ajustado para armazenar 7 bits + '\0'
    char sequencia[100] = ""; //string para a função sequencia_ean8
    int i;

    //valores padrão
    int espaco_lateral = 10;
    int pixels_por_area = 1;
    int altura = 50;
    char nome_da_imagem[50] = "codigo_barra.pbm";

    // Atualizar os valores se forem fornecidos
    if (argc > 2) espaco_lateral = atoi(argv[2]);
    if (argc > 3) pixels_por_area = atoi(argv[3]);
    if (argc > 4) altura = atoi(argv[4]);
    if (argc > 5) strncpy(nome_da_imagem, argv[5], 50);

    if (strlen(entrada) != 8) {
        printf("Voce deve inserir exatamente 8 digitos!\n");
        return 1;
    }

    for (i = 0; i < 8; i++) {
        if (entrada[i] < '0' || entrada[i] > '9') {
            printf("Voce deve digitar apenas numeros!\n");
            return 1;
        }
        // Troca char por int
        digito_id[i] = entrada[i] - '0';
    }

    // Conversão dos 4 primeiros números usando a tabela fornecida
    for (i = 0; i < 4; i++) {
        memset(LCode[i], 0, sizeof(LCode[i])); // Zera o buffer
        converter_LCode(digito_id[i], LCode[i]);
    }

    // Conversão dos 4 últimos números usando a tabela fornecida
    for (i = 0; i < 4; i++) {
        memset(RCode[i], 0, sizeof(RCode[i])); // Zera o buffer
        converter_RCode(digito_id[i + 4], RCode[i]);
    }

    // Exibe os resultados em binário (LCode)
    printf("Os primeiros 4 digitos em binario (LCode):\n");
    for (i = 0; i < 4; i++) {
        printf("%s ", LCode[i]);
    }
    printf("\n");

    // Exibe os resultados em binário (RCode)
    printf("Os ultimos 4 digitos em binario (RCode):\n");
    for (i = 0; i < 4; i++) {
        printf("%s ", RCode[i]); // Cada elemento de RCode é exibido exatamente uma vez
    }
    printf("\n");

    // Testando para saber se o dígito id é válido ou não
    if (!validar_digito_id(digito_id)) {
    printf("Erro: O digito verificador fornecido (%d) e invalido.\n", digito_id[7]);
    return 1;
    }
    else{
        printf("Digito Verificador Valido\n"); 
    }

    //mostrando a sequencia do código de barras
    sequencia_ean8(digito_id, sequencia);
    printf("Sequencia do codigo de barras: %s\n", sequencia);

    // Dizendo qual o dígito verificador
    printf("Digito verificador: %d\n", calculo_digito_verificador(digito_id));

   
    //Exibir informações adicionais
    printf("espaco lateral: %d\n", espaco_lateral);
    printf("pixels p area: %d\n", pixels_por_area);
    printf("altura: %d\n", altura);
    printf("nome da imagem: %s\n", nome_da_imagem);

    //gerando o arquivo PBM
    gerar_pbm(sequencia, pixels_por_area, altura, espaco_lateral, nome_da_imagem);

    return 0;
}
