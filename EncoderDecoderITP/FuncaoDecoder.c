#include <stdio.h>
#include <stdlib.h> 
#include <string.h>



//função para verificar se o arquivo PBM é válido
int verificar_arquivo_PBM(const char *nome_arquivo ){
    
    FILE *arquivo = fopen(nome_arquivo, "r");
    if(!arquivo){
        printf("Erro ao abrir o arquivo!");
        return 0;
    }

    char formato_imagem_PBM[3];
    int largura_total, altura_total; 

    //ler um texto de até 2 caractere e armazena em formato, verifica se leu pelo menos 1 caractere ou se formato e P1 são iguais, caso não o PBM é inválido
    if(fscanf(arquivo, "%2s", formato_imagem_PBM) != 1 || strcmp(formato_imagem_PBM, "P1")){
        printf("Formato PBM inválido.\n");
        fclose(arquivo);
        return 0;
    }
    

    // Ignorar possíveis comentários no cabeçalho
    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (linha[0] == '#') {
            continue; // Ignora linhas de comentário
        }
        // Se a linha não for um comentário, tenta ler a largura e altura
        if (sscanf(linha, "%d %d", &largura_total, &altura_total) == 2) {
            if (largura_total <= 0 || altura_total <= 0) {
                printf("Dimensões inválidas no cabeçalho do arquivo.\n");
                fclose(arquivo);
                return 0;
            }
            break;
        }
    }

    fclose(arquivo);
    return 1;
}


//Função do dígito verificador
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


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <arquivo PBM>\n", argv[0]);
        return 1;
    }

    const char *arquivoPBM = argv[1];
    

    // Verificar se o arquivo PBM é válido
    if (!verificar_arquivo_PBM(arquivoPBM)) {
        printf("Arquivo PBM invalido ou inexistente.\n");
        return 1;
    }
    else{
        printf("Arquivo PBM valido.\n"); 
    }

    int digito_id[8];

    int digitoVerificador = calculo_digito_verificador(digito_id);
    if (digitoVerificador != digito_id[7]) {
        printf("Digito Verificador Invalido.\n");
        return 1;
    }
    else{
        printf("Digito Verificador valido.\n"); 
    }

}