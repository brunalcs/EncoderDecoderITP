#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Tabelas de conversão para L-code e R-code
const char *Lcode[10] = {
    "0001101", "0011001", "0010011", "0111101", "0100011",
    "0110001", "0101111", "0111011", "0110111", "0001011"
};

const char *Rcode[10] = {
    "1110010", "1100110", "1101100", "1000010", "1011100",
    "1001110", "1010000", "1000100", "1001000", "1110100"
};


// Função para interpretar múltiplos pixels como um único bit
char interpretarBits(const char *pixels, int tamanho) {
    int contadorPreto = 0;
    for (int i = 0; i < tamanho; i++) {
        if (pixels[i] == '1') {
            contadorPreto++;
        }
    }
    return (contadorPreto > tamanho / 2) ? '1' : '0';
}

// Função para verificar se o arquivo PBM é válido
int verificar_arquivo_PBM(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo!\n");
        return 0;
    }

    char formato_imagem_PBM[3];
    int largura_total, altura_total;

    if (fscanf(arquivo, "%2s", formato_imagem_PBM) != 1 || strcmp(formato_imagem_PBM, "P1")) {
        printf("Formato PBM invalido.\n");
        fclose(arquivo);
        return 0;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (linha[0] == '#') continue; // Ignorar comentários
        if (sscanf(linha, "%d %d", &largura_total, &altura_total) == 2) {
            if (largura_total <= 0 || altura_total <= 0) {
                printf("Dimensões invalidas no cabeçalho do arquivo.\n");
                fclose(arquivo);
                return 0;
            }
            break;
        }
    }

    fclose(arquivo);
    return 1;
}

// Função para remover os zeros à esquerda até o primeiro '1' e os zeros à direita após o último '1'
void remover_zeros_externos(char *linha) {
    // Remover zeros à esquerda
    char *primeiro_um = strchr(linha, '1'); // Encontra o primeiro '1'
    if (primeiro_um) {
        // Move a string a partir do primeiro '1'
        memmove(linha, primeiro_um, strlen(primeiro_um) + 1);
    } else {
        // Se não houver '1', a linha será vazia
        linha[0] = '\0';
        return;
    }

    // Remover zeros à direita
    char *ultimo_um = strrchr(linha, '1'); // Encontra o último '1'
    if (ultimo_um) {
        // Termina a string logo após o último '1'
        *(ultimo_um + 1) = '\0';
    }
}

// Função para verificar se a linha contém apenas zeros
int linha_somente_zeros(const char *linha) {
    for (int i = 0; linha[i] != '\0'; i++) {
        if (!isspace(linha[i]) && linha[i] != '0') {
            return 0; // Contém algo diferente de '0' ou espaços
        }
    }
    return 1; // Apenas zeros (ou espaços)
}

// Função para processar a linha baseada em pixel_area
void processar_linha_pixel_area(const char *linha_processada, int pixel_area) {
    char nova_string[256] = ""; // String para armazenar os valores resultantes

    if (pixel_area <= 1) {
        // Caso pixel_area seja 1 ou menor, copia a linha original sem espaços extras
        strcpy(nova_string, linha_processada);
    } else {
        int comprimento = strlen(linha_processada);
        int pos = 0;

        for (int i = 0; i < comprimento; i += pixel_area) {
            nova_string[pos++] = linha_processada[i]; // Adiciona o caractere atual
        }
        nova_string[pos] = '\0'; // Finaliza a string
    }

    printf("Nova string processada: %s\n", nova_string);

    // Dividir a nova_string nos intervalos especificados
    dividir_string(nova_string);
}

// Função para localizar o código na tabela
int localizarCodigo(const char *binario, const char *tabela[10]) {
    for (int i = 0; i < 10; i++) {
        if (strcmp(binario, tabela[i]) == 0) {
            return i;
        }
    }
    return -1; // Se não encontrar
}

// Função para converter segmentos de 7 bits usando Lcode e Rcode
// Função para converter segmentos de 7 bits usando Lcode e Rcode
void converterSegmentos(char segmentos_7_indices[][7][256], int num_segmentos) {
    char resultadoFinal[64] = ""; // String para armazenar o resultado final

    for (int i = 0; i < num_segmentos; i++) {
        // Obtenha o segmento atual com 7 índices
        char binario[8];
        strcpy(binario, segmentos_7_indices[i][0]);

        // Verifique se o segmento pertence a Lcode ou Rcode
        int numero = -1;
        if (i < 4) { // Primeiros 4 segmentos: usar Lcode
            numero = localizarCodigo(binario, Lcode);
        } else { // Últimos 4 segmentos: usar Rcode
            numero = localizarCodigo(binario, Rcode);
        }

        if (numero != -1) {
            // Adicione o número ao resultado final
            sprintf(resultadoFinal + strlen(resultadoFinal), "%d", numero);
        } else {
            printf("Erro ao encontrar o código para o segmento: %s\n", binario);
        }
    }

    printf("Resultado Final: %s\n", resultadoFinal);

    // Calcular o dígito verificador
    int digito_id[7]; 
    for (int i = 0; i < 7; i++) {
        digito_id[i] = resultadoFinal[i] - '0'; // Converte cada caractere em número
    }

    int digitoVerificadorCalculado = calculo_digito_verificador(digito_id);
    printf("\nDigito Verificador Calculado: %d\n\n", digitoVerificadorCalculado);

    // Comparar com o dígito verificador da string
    int digitoVerificadorExtraido = resultadoFinal[7] - '0'; // O 8º dígito da string
    if (digitoVerificadorCalculado == digitoVerificadorExtraido) {
        printf("Digito verificador valido.\n\n");
    } else {
        printf("Digito verificador invalido.\n\n");
    }
}

// Função para dividir a nova_string nos intervalos
void dividir_string(const char *nova_string) {
    // Define os intervalos
    int intervalos[] = {3, 7, 7, 7, 7, 5, 7, 7, 7, 7, 3};
    int num_intervalos = sizeof(intervalos) / sizeof(intervalos[0]);

    // Vetor para armazenar os segmentos
    char segmentos[num_intervalos][256];
    memset(segmentos, 0, sizeof(segmentos)); // Inicializa o vetor com zeros

    // Matriz para armazenar segmentos com 7 índices
    char segmentos_7_indices[num_intervalos][7][256];
    int indice_segmentos_7_indices = 0;

    int pos = 0; // Posição atual na string
    int comprimento = strlen(nova_string);

    // Divide a string conforme os intervalos
    for (int i = 0; i < num_intervalos; i++) {
        int intervalo = intervalos[i];

        // Verifica se há caracteres suficientes para o intervalo
        if (pos + intervalo <= comprimento) {
            strncpy(segmentos[i], nova_string + pos, intervalo);
            segmentos[i][intervalo] = '\0'; // Finaliza a string do segmento
            pos += intervalo; // Atualiza a posição

            // Se o segmento tem 7 índices, armazene na matriz segmentos_7_indices
            if (intervalo == 7) {
                strncpy(segmentos_7_indices[indice_segmentos_7_indices][0], segmentos[i], 256);
                indice_segmentos_7_indices++;
            }
        } else {
            printf("Intervalo excede o comprimento da string. Interrompendo divisão.\n");
            break;
        }
    }

    // Imprime os segmentos
    printf("Segmentos divididos:\n");
    for (int i = 0; i < num_intervalos; i++) {
        if (strlen(segmentos[i]) > 0) {
            printf("Segmento %d: %s\n", i + 1, segmentos[i]);
        }
    }

    // Imprime os segmentos com 7 índices armazenados na matriz
    printf("\nSegmentos com 7 indices\n");
    for (int i = 0; i < indice_segmentos_7_indices; i++) {
        printf("Segmento com 7 indices %d: %s\n", i + 1, segmentos_7_indices[i][0]);
    }
    // Converter os segmentos para valores decimais
    converterSegmentos(segmentos_7_indices, indice_segmentos_7_indices);
}

// Função para ler a linha binária sem zeros externos e calcular pixel_area
void ler_linha_binaria(const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo) {
        printf("Erro ao abrir o arquivo!\n");
        return;
    }

    char linha[256];
    int encontrou_cabecalho = 0;
    char linha_processada[256] = ""; // String para armazenar a linha processada

    // Ignorar cabeçalho e comentários
    while (fgets(linha, sizeof(linha), arquivo)) {
        if (linha[0] == '#') continue; // Ignorar comentários
        if (!encontrou_cabecalho) {
            int largura, altura;
            if (sscanf(linha, "%d %d", &largura, &altura) == 2) {
                encontrou_cabecalho = 1;
            }
            continue;
        }

        // Verificar se a linha contém apenas zeros
        if (linha_somente_zeros(linha)) {
            continue; // Ignorar linha apenas com zeros
        }

        // Lê a linha dos binários
        if (encontrou_cabecalho) {
            remover_zeros_externos(linha); // Remove os zeros externos
            strcpy(linha_processada, linha); // Armazena a linha processada
            printf("Linha dos binarios sem zeros externos: %s\n\n", linha_processada);

            // Calcula pixel_area
            int comprimento = strlen(linha_processada);
            int pixel_area = comprimento / 67;
            printf("Pixel Area: %d\n\n", pixel_area);

            // Processa a linha baseada no pixel_area
            processar_linha_pixel_area(linha_processada, pixel_area);

            break;
        }
    }

    fclose(arquivo);
}

// Função do dígito verificador
int calculo_digito_verificador(int digito_id[7]) {
    int soma = 0;
    for (int i = 0; i < 7; i++) {
        soma += digito_id[i] * (i % 2 == 0 ? 3 : 1);
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
        printf("Arquivo PBM invalido ou inexistente.\n\n");
        return 1;
    } else {
        printf("Arquivo PBM valido.\n\n");
    }

    ler_linha_binaria(arquivoPBM);

    return 0;
}
