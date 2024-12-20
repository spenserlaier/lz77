#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int backwards_offset;
    int length;
    char character;
} lz77_triplet;
const int DEFAULT_BUFF_SIZE = 50;

int max(int n1, int n2) {
    return n1 > n2? n1 : n2;
}

typedef struct {
    char * string;
    int length;
} lz77;

lz77 * encode_lz77(char* input_string, int search_buff_size) {
    printf("Beginning encoding\n");
    unsigned long input_length = strlen(input_string);
    if (input_length < 2*search_buff_size) {
        return NULL;
    }
    //TODO: error catching on malloc
    char * output_string = malloc(sizeof(char) * input_length * 3);
    int output_string_idx = 0;
    int search_buff_left = 0;
    int search_buff_right = search_buff_size-1;
    int look_ahead_buff_left = search_buff_right + 1;
    int look_ahead_buff_right = look_ahead_buff_left;

    //output first <window size> triplets as we initialize the search buffer.
    // triplet structure: <backwards_offset, match_length, character>
    for (int idx = 0; idx <= search_buff_right; idx ++ ) {
        output_string[output_string_idx++] = 0;
        output_string[output_string_idx++] = 0;
        output_string[output_string_idx++] = input_string[idx];
    }
    //while (look_ahead_buff_right < input_length) {
    while (look_ahead_buff_left < input_length) {
        //iterate through the  search buffer looking for the longest match starting
        //at current character in lookahead buffer
        //printf("look_ahead_right: %d input_length: %lu\n", look_ahead_buff_right, input_length);
        //printf("look_ahead_left: %d input_length: %lu\n", look_ahead_buff_left, input_length);
        //printf("look_ahead_right: %d input_length: %lu\n", look_ahead_buff_right, input_length);
        int longest_match_length = 0;
        int longest_match_backwards_offset = 0;
        for (int idx = search_buff_left; idx <= search_buff_right; idx ++) {
            int current_match_length = 0;
            int match_idx_left = idx;
            int match_idx_right = look_ahead_buff_left;
            while (input_string[match_idx_left] == input_string[match_idx_right]
                    && match_idx_left <= search_buff_right
                    && match_idx_right < look_ahead_buff_left + DEFAULT_BUFF_SIZE
                    && match_idx_right < input_length
                  )
            {
                match_idx_left ++;
                match_idx_right ++;
                current_match_length ++;
            }
            if  (current_match_length > longest_match_length) {
                longest_match_length = current_match_length;
                longest_match_backwards_offset = look_ahead_buff_left - idx;
            }
        }
        // if we have a match, output a triplet and shift the buffers right by the size of the match
        printf("longest match length found at look_ahead_left %d : %d\n", look_ahead_buff_left, longest_match_length);
        output_string[output_string_idx++] = longest_match_backwards_offset;
        output_string[output_string_idx++] = longest_match_length;
        output_string[output_string_idx++] = input_string[look_ahead_buff_left];

        int skip_size = longest_match_length == 0? 1 : longest_match_length;
        //int skip_size = longest_match_length + 1;
        look_ahead_buff_left += skip_size;
        search_buff_left += skip_size;
        search_buff_right += skip_size;
        if (look_ahead_buff_left > input_length) {
            //TODO: we're matching against an extra character at the end it seems.
            printf("jumped buffer ahead to %d, from prior position %d, original input length: %lu\n", look_ahead_buff_left, look_ahead_buff_left - skip_size, input_length);
            printf("longest match was %d\n", longest_match_length);
        }
    }
    //TODO: setting a char to 0 is equivalent to setting it to null terminator \0, which
    //creates issues when searching for the end of a string
    lz77 * output = malloc(sizeof(lz77));
    output->length = output_string_idx;
    output->string = output_string;
    printf("Encoding finished with output length at %d.\n", output_string_idx);
    return output;
}
char * decode_lz77(lz77 * encoded_input) {
    printf("Beginning decoding\n");
    //scan through the match length values of the output and add them together
    //to determine the decoded output. in cases where match length is 0, add 1
    int decoded_size = 0;
    int idx = 1; // position of first match_size value
    while (idx < encoded_input->length) {
        int match_size = encoded_input->string[idx] == 0? 1 : encoded_input->string[idx];
        decoded_size += match_size;
        idx += 3;
    }
    char * decoded = malloc(sizeof(char)*(decoded_size + 1));
    int decoded_idx = 0;
    for (int i = 0; i< encoded_input->length; i += 3) {
        char backwards_offset = encoded_input->string[i];
        char match_length = encoded_input->string[i+1];
        char character = encoded_input->string[i+2];
        if (match_length != 50 && match_length != 0 ) {
            printf("special match length %d found at index %d\n", match_length, decoded_idx);

        }
        if (match_length != 0) {
            int backwards_index = decoded_idx - backwards_offset;
            for (int j = backwards_index; j < backwards_index + match_length; j ++ ) {
                if (decoded[j] == '\0') {
                    printf("trouble: null terminator found in middle of string, idx : %d\n", decoded_idx);
                    printf("decoded index: %d, backwards index: %d initial backwards\n", decoded_idx, j);
                }
                decoded[decoded_idx++] = decoded[j];
            }
        } else {
            if (character == '\0') {
                printf("direct character assignment trouble: null terminator found in middle of string, idx : %d\n", decoded_idx);
            }
            //printf("performing direct character assignment at index %d\n", decoded_idx);
            decoded[decoded_idx++] = character;
        }
    }
    // printf("Printing last triplet: \n");
    // printf("offset: %c\n", encoded_input->string[encoded_input->length-3]);
    // printf("match length: %d\n",encoded_input->string[encoded_input->length-2]);
    // printf("character: %c\n",encoded_input->string[encoded_input->length-1]);
    // printf("Decoding finished with index positioned at: %d\n", decoded_idx );
    decoded[decoded_idx] = '\0';
    //printf("Decoding successful\n");
    return decoded;
}
int run_test(char * test_string) {
    lz77 * encoded = encode_lz77(test_string, DEFAULT_BUFF_SIZE);
    char * decoded = decode_lz77(encoded);
    int cmp_result = strcmp(decoded, test_string);
    printf("length of input: %lu length of output: %lu\n", strlen(test_string), strlen(decoded));
    //printf("original string: %s\n", test_string);
    //printf("decoded string: %s\n", decoded);
    return cmp_result;
}
char * generate_uniform_string(char character, int length) {
    char * output = malloc(sizeof(char) * length);
    for (int i = 0; i < length; i++ ) {
        output[i] = character;
    }
    output[length-1] = '\0';
    return output;
}

int main() {
    char * uniform_a_500 = generate_uniform_string('a', 500);
    printf("Result of testing 500 uniform a's: %d\n", run_test(uniform_a_500));
    char * uniform_a_501 = generate_uniform_string('a', 501);
    printf("Result of testing 501 uniform a's: %d\n", run_test(uniform_a_501));
    char * uniform_a_502 = generate_uniform_string('a', 502);
    printf("Result of testing 502 uniform a's: %d\n", run_test(uniform_a_502));
    char * uniform_a_505 = generate_uniform_string('a', 505);
    printf("Result of testing 505 uniform a's: %d\n", run_test(uniform_a_505));
    char * uniform_a_515 = generate_uniform_string('a', 515);
    printf("Result of testing 515 uniform a's: %d\n", run_test(uniform_a_515));
    return 0;
}
