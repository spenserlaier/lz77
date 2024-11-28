#include <stdio.h>
#include <stdlib.h>

char ENCODING_END = -1;


char * lz77(char * input, size_t input_size, int buffer_size) {
    char * output = malloc(3*input_size);
    int search_left = 0;
    int search_right = 0;
    int lookahead_left = 0;
    int lookahead_right = 0;
    int output_idx = 0;
    //triplet format: <offset, length, character>
    while (lookahead_right < buffer_size/2) {
        char curr = input[lookahead_right];

        output[output_idx] = 0;
        output_idx ++;
        output[output_idx] = 0;
        output_idx ++;
        output[output_idx] = curr;
        output_idx ++;

        lookahead_right ++;
    }
    lookahead_left ++;
    //printf("left: %d right: %d\n", lookahead_left, lookahead_right);
    while (lookahead_right < input_size) {
        char max_match_length = 0;
        char match_offset = 0;
        char match_char = input[lookahead_right];
        //printf("lookahead left: %d lookahead right: %d\n", lookahead_left, lookahead_right);
        //printf("search left: %d search right: %d\n", search_left, search_right);

        for (int search_idx = search_left; 
                search_idx <= search_right; 
                search_idx ++) {
            //iterate through search buffer looking for longest match against 
            //lookahead buffer prefix
            int match_length = 0;

            while (search_idx + match_length <= search_right &&
                    lookahead_left + match_length <= lookahead_right &&
                    input[search_idx + match_length] == input[lookahead_left + match_length]){
                match_length ++;
                //TODO: make sure to skip buffers ahead by the length of the longest match
            }
            if (match_length > max_match_length) {
                max_match_length = match_length;
                match_offset = lookahead_left - search_idx;
            }

        }

        output[output_idx] = match_offset;
        output_idx ++;
        output[output_idx] = max_match_length;
        output_idx ++;
        output[output_idx] = match_char;
        output_idx ++;

        lookahead_left ++;
        lookahead_right ++;
        search_right ++;
        if (search_right - search_left +1 > buffer_size/2) {
            search_left++;
        }
    }
    output[output_idx] = ENCODING_END;
    return output;
}

void print_triplets(char * input) {
    int idx = 0;
    while (input[idx] != ENCODING_END) {
        printf("printing triplet: \n");
        printf("length:  %d\n", input[idx]);
        printf("offset: %d\n", input[idx+1]);
        printf("char: %c\n", input[idx+2]);
        idx +=3;
    }
}


int main() {
    int test_input_size = 300;
    char * test_input = malloc(test_input_size);
    for (int i = 0; i < test_input_size; i++ ){
        test_input[i] = 'a';
    }
    char * output = lz77(test_input, test_input_size, 50);
    print_triplets(output);
    return 0;
}
