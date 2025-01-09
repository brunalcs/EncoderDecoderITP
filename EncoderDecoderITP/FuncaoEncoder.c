
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

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
    int calculo_digito_id = calculo_digito_verificador(digito_id);

    return calculo_digito_id == digito_id[7];
}

// Função para converter os 4 primeiros números usando a tabela LCode
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
        converter_RCode(digito_id[i], numeros_binarios);
        strcat(sequencia, numeros_binarios);
    }

    //marcador final
    strcat(sequencia, "101");

}


int main() {
    int digito_id[8];
    char entrada[9];
    char LCode[4][8]; // Tamanho ajustado para armazenar 7 bits + '\0'
    char RCode[4][8]; // Tamanho ajustado para armazenar 7 bits + '\0'
    char sequencia [100] = ""; //string para a função sequencia_ean8
    int i;

    printf("Digite os 8 dígitos: ");
    scanf("%8s", entrada);

    if (strlen(entrada) != 8) {
        printf("Você deve inserir apenas 8 dígitos!\n");
        return 1;
    }

    // Limpa buffer
    while (getchar() != '\n');

    for (i = 0; i < 8; i++) {
        if (entrada[i] < '0' || entrada[i] > '9') {
            printf("Você deve digitar apenas números!\n");
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
    printf("Os primeiros 4 dígitos em binário (LCode):\n");
    for (i = 0; i < 4; i++) {
        printf("%s ", LCode[i]);
    }
    printf("\n");

    // Exibe os resultados em binário (RCode)
    printf("Os últimos 4 dígitos em binário (RCode):\n");
    for (i = 0; i < 4; i++) {
        printf("%s ", RCode[i]); // Cada elemento de RCode é exibido exatamente uma vez
    }
    printf("\n");

    //mostrando a sequencia do código de barras
    sequencia_ean8(digito_id, sequencia);
    printf("Sequência do código de barras: %s\n", sequencia);


    // Dizendo qual o dígito verificador
    printf("O dígito verificador é %d\n", calculo_digito_verificador(digito_id));

    // Testando
    if (validar_digito_id(digito_id)) {
        printf("Dígito Verificador Válido\n");
    } else {
        printf("Dígito Verificador Inválido\n");
    }

    return 0;
}
