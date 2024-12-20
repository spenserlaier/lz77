#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    int backwards_offset;
    int length;
    char character;
} lz77_triplet;

int max(int n1, int n2) {
    return n1 > n2? n1 : n2;
}

typedef struct {
    char * string;
    int length;
} lz77_output;

lz77_output * encode_lz77(char* input_string, int search_buff_size) {
    unsigned long input_length = strlen(input_string);
    if (input_length < 2*search_buff_size) {
        return NULL;
    }
    char * output_string = malloc(sizeof(char) * input_length * 3);
    int output_string_idx = 0;
    int search_buff_left = 0;
    int search_buff_right = search_buff_size - 1;
    int look_ahead_buff_left = search_buff_right + 1;
    int look_ahead_buff_right = look_ahead_buff_left;

    int longest_match_length = 0;
    int longest_match_backwards_offset = 0;
    //output first <window size> triplets as we initialize the search buffer.
    // triplet structure: <backwards_offset, match_length, character>
    for (int idx = 0; idx < search_buff_right; idx ++ ) {
        output_string[output_string_idx++] = 0;
        output_string[output_string_idx++] = 0;
        output_string[output_string_idx++] = input_string[idx];
    }
    while (look_ahead_buff_right < input_length) {
        //iterate through the  search buffer looking for the longest match starting
        //at current character in lookahead buffer
        for (int idx = search_buff_left; idx < search_buff_right; idx ++) {
            int current_match_length = 0;
            int match_idx_left = idx;
            int match_idx_right = search_buff_right;
            while (input_string[match_idx_left] == input_string[match_idx_right]
                    && match_idx_left < search_buff_right)
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
        output_string[output_string_idx++] = longest_match_backwards_offset;
        output_string[output_string_idx++] = longest_match_length;
        output_string[output_string_idx++] = input_string[look_ahead_buff_left];

        int skip_size = longest_match_length == 0? 1 : longest_match_length;
        look_ahead_buff_left += skip_size;
        search_buff_left += skip_size;
        search_buff_right += skip_size;
    }
    //TODO: setting a char to 0 is equivalent to setting it to null terminator \0, which
    //creates issues when searching for the end of a string
    lz77_output * output = malloc(sizeof(lz77_output));
    output->length = output_string_idx;
    output->string = output_string;
    return output;
}
char * decode_lz77(lz77_output * encoded_input) {
    //scan through the match length values of the output and add them together
    //to determine the decoded output. in cases where match length is 0, add 1
    int decoded_size = 0;
    int idx = 1; // position of first match_size value
    while (idx < encoded_input->length) {
        int match_size = encoded_input->string[idx] == 0? 1 : encoded_input->string[idx];
        decoded_size += match_size;
        idx += 3;
    }
    char * decoded = malloc(sizeof(char)*decoded_size);
    int decoded_idx = 0;
    for (int i = 0; i< encoded_input->length; i += 3) {
        char backwards_offset = encoded_input->string[i];
        char match_length = encoded_input->string[i+1];
        char character = encoded_input->string[i+2];
        if (match_length != 0) {
            for (int j = i - backwards_offset; j < i + match_length; j ++ ) {
                decoded[decoded_idx++] = decoded[j];
            }
        } else {
            decoded[decoded_idx++] = character;
        }
    }
    return decoded;
}

int main() {
    return 0;
}
