#include <iostream>
#include <string> 
#include <fstream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <map>
#include <algorithm>
#include <queue>


std::map<char, int> convert_unordered_to_map(const std::unordered_map<char, int>& unordered_map) {
    std::map<char, int> ordered_map;
    for (const auto& pair : unordered_map) {
        ordered_map.insert(pair); // или ordered_map[pair.first] = pair.second;
    }
    return ordered_map;
}

struct HuffmanNode 
{
    char ch;
    int freq;
    HuffmanNode *left, *right;

    HuffmanNode(char ch, int freq) : ch(ch), freq(freq), left(nullptr), right(nullptr){}
};

struct compareNodes {
    bool operator()(HuffmanNode* l, HuffmanNode* r) {
        return (l->freq > r->freq);
    }
};

// Function to build Huffman tree
HuffmanNode* buildHuffmanTree(std::map<char, int>& freq) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, compareNodes> pq;

    for (auto pair : freq) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() != 1) {
        HuffmanNode *left = pq.top(); pq.pop();
        HuffmanNode *right = pq.top(); pq.pop();

        HuffmanNode *top = new HuffmanNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        pq.push(top);
    }
    return pq.top();
}


void printCodes(HuffmanNode* root, std::string str, std::map<char, std::string>& huffmanCode) {
    if (!root)
        return;

    if (root->left == nullptr && root->right == nullptr) {
        huffmanCode[root->ch] = str;
        return;
    }

    printCodes(root->left, str + "0", huffmanCode);
    printCodes(root->right, str + "1", huffmanCode);
}

std::map<std::string, int> calculate_frequency_shannon(const std::vector<std::string>& blocks) {
    std::map<std::string, int> frequency;
    for (const auto& block : blocks) {
        frequency[block]++;
    }
    return frequency;
}

// Function to calculate frequency for Huffman (for characters)
std::map<char, int> calculate_frequency_huffman(const std::string& text) {
    std::map<char, int> frequency;
    for (char c : text) {
        frequency[c]++;
    }
    return frequency;
}


class Block {
public:
    std::string characters;
    double probability; 

    Block(const std::string& chars, double p) : characters(chars), probability(p) {}
};

//подсчет частоты символа
std::unordered_map<char, int> calculate_frequency(const std::string& text) {
    std::unordered_map<char, int> frequency;
    for (char c : text) {//fhhhh
        frequency[c]++;
    }
    return frequency;
}
// Функция для расчета энтропии
double calculate_entropy(const std::map<char, int>& frequency, int total_chars) {
    double entropy = 0.0;
    for (const auto& pair : frequency) {
        double prob = static_cast<double>(pair.second) / total_chars;
        if (prob > 0) {
            entropy -= prob * std::log2(prob);
        }
    }
    return entropy;
}

//функция для разбиения на блоки 
std::vector<std::string> split_into_blocks(const std::string& text, int block_length) {
    std::vector<std::string> blocks;
    for (size_t i = 0; i < text.size(); i += block_length) {
        blocks.push_back(text.substr(i, block_length));
    }
    return blocks;
}


//нахождение оптимальной длины
int optimal_block_length(const std::string& text) {
    auto unordered_frequency = calculate_frequency(text);
    auto frequency = convert_unordered_to_map(unordered_frequency); // Преобразуем в std::map
    int total_chars = text.size();

    if (total_chars == 0) return 1; // Защита от деления на ноль

    double entropy = calculate_entropy(frequency, total_chars);
    return static_cast<int>(std::pow(2, entropy));
}


std::string read_text(std::string filename){
    std::ifstream inputFile(filename);
    std::string text;

    if (inputFile.is_open())
    {
        std::string line;
        while(std::getline(inputFile, line)){
            text += line;
        }
        inputFile.close();
    }
    else
    {
        std::cerr << "Не удалось открыть файл " << std::endl;
    }
    return text;
} 

bool compareBlocks(const Block& a, const Block& b) {
    return a.probability > b.probability; // Сортировка по убыванию вероятности
}

// Функция для генерации кодов Шеннона-Фано
void generateShannonFanoCodes(const std::vector<Block>& blocks, std::map<std::string, std::string>& codes, const std::string& code) {
    if (blocks.size() == 1) {
        codes[blocks[0].characters] = code; // Присваиваем код единственному блоку
        return;
    }

    double total = 0.0;
    for (const auto& block : blocks) {
        total += block.probability;
    }

    double cumulative = 0.0;
    size_t splitIndex = 0;
    double minDifference = total; // Инициализируем минимальную разницу

    for (size_t i = 0; i < blocks.size() - 1; ++i) {
        cumulative += blocks[i].probability;
        double difference = std::abs(total / 2 - cumulative);

        if (difference < minDifference){
            minDifference = difference;
            splitIndex = i + 1; // Индекс первого элемента второй половины
        }
    }

    generateShannonFanoCodes(std::vector<Block>(blocks.begin(), blocks.begin() + splitIndex), codes, code + "0");
    generateShannonFanoCodes(std::vector<Block>(blocks.begin() + splitIndex, blocks.end()), codes, code + "1");
}

void write_encoded_blocks_to_file(const std::map<std::string, std::string>& codes, const std::string& output_filename) {
    std::ofstream outputFile(output_filename);
    
    if (outputFile.is_open()) {
        for (const auto& pair : codes) {
            outputFile << pair.first << ": " << pair.second << "\n";
        }
        outputFile.close();
        std::cout << "Закодированные блоки успешно записаны в файл: " << output_filename << std::endl;
    } else {
        std::cerr << "Не удалось открыть файл для записи: " << output_filename << std::endl;
    }
}

void write_encoded_blocks_to_file(const std::map<char, std::string>& codes, const std::string& output_filename) {
    std::ofstream outputFile(output_filename);
    
    if (outputFile.is_open()) {
        for (const auto& pair : codes) {
            outputFile << pair.first << ": " << pair.second << "\n";
        }
        outputFile.close();
        std::cout << "Закодированные блоки успешно записаны в файл: " << output_filename << std::endl;
    } else {
        std::cerr << "Не удалось открыть файл для записи: " << output_filename << std::endl;
    }
}

void read_encoded_data(const std::string& filename, std::map<std::string, std::string>& reverse_codes) {
    std::ifstream inputFile(filename);
    
    if (inputFile.is_open()) {
        std::string line;
        while (std::getline(inputFile, line)) {
            size_t separator_pos = line.find(": "); // Ищем позицию ": "
            if (separator_pos != std::string::npos) {
                std::string symbol = line.substr(0, separator_pos); // Символы перед ": "
                std::string code = line.substr(separator_pos + 2); // Код после ": "
                reverse_codes[code] = symbol; // Заполняем обратную карту
            }
        }
        inputFile.close();
        std::cout << "Закодированные данные успешно прочитаны из файла: " << filename << std::endl;
    } else {
        std::cerr << "Не удалось открыть файл для чтения: " << filename << std::endl;
    }
}


// Функция для декодирования закодированной строки
std::string decode(const std::string& encoded_string, const std::map<std::string, std::string>& reverse_codes) {
    std::string decoded_string;
    std::string current_code;

    for (char bit : encoded_string) {
        current_code += bit; // Добавляем бит к текущему коду

        // Проверяем, есть ли текущий код в обратной карте
        if (reverse_codes.find(current_code) != reverse_codes.end()) {
            decoded_string += reverse_codes.at(current_code); // Добавляем символ к декодированной строке
            current_code.clear(); // Очищаем текущий код
        }
    }

    return decoded_string;
}

std::string read_binary_sequence(const std::string filename) {
    std::ifstream inputFile(filename);
    std::string sequence;

    if (inputFile.is_open()) {
        inputFile >> sequence; // Чтение всей строки с последовательностью
        inputFile.close();
    } else {
        std::cerr << "Не удалось открыть файл " << filename << std::endl;
    }
    
    return sequence;
}

void write_decoded_string_to_file(const std::string& decoded_string, const std::string& output_filename) {
    std::ofstream outputFile(output_filename);
    
    if (outputFile.is_open()) {
        outputFile << decoded_string; // Запись декодированной строки в файл
        outputFile.close();
        std::cout << "Декодированная строка успешно записана в файл: " << output_filename << std::endl;
    } else {
        std::cerr << "Не удалось открыть файл для записи: " << output_filename << std::endl;
    }
}

std::string decode_huffman(const std::string& encoded_sequence, HuffmanNode* root) {
    std::string decoded_string;
    HuffmanNode* current = root;

    for (char bit : encoded_sequence) {
        if (bit == '0') {
            current = current->left;
        } else {
            current = current->right;
        }

        if (!current->left && !current->right) { // Если достигли листа дерева
            decoded_string += current->ch; 
            current = root; // Возвращаемся к корню дерева для следующего символа
        }
    }

    return decoded_string;
}

int main(){
    const std::string encoded_filename = "D:/proga/teor_inf_thrid/encoded_output_new.txt";
    const std::string encoded_filename_2 = "D:/proga/teor_inf03/encoded_output_huffmen.txt";//файл с закодированными данными 
    const std::string input_filename = "D:/proga/teor_inf_thrid/input.txt";
    const std::string binary_sequence_filename = "D:/proga/teor_inf03/input2.txt";
    const std::string binary_sequence_filename_2 = "D:/proga/teor_inf03/input2_forHaf.txt";
    const std::string output_filename = "D:/proga/teor_inf03/encoded_output2.txt";
    const std::string output_filename2 = "D:/proga/teor_inf03/encoded_output22.txt";
    const std::string output_filenameHUF = "D:/proga/teor_inf03/HUF.txt";
    std::string change;

    std::cout << "1 - Заархивировать с помощью Шеннона - Фано " << std::endl;
    std::cout << "2 - Разархивировать с помщью Шеннона - Фано " << std::endl;
    std::cout << "3 - Заархивировать с помощью Хаффмена " << std::endl;
    std::cout << "4 - Разархивировать с помощью Хаффмена " << std::endl;
    std::cin >> change;

    if (change == "1") {
        std::string text_in_file = read_text(input_filename);
        int block_lenght = optimal_block_length(text_in_file);
        auto blocks_str = split_into_blocks(text_in_file, block_lenght);

        auto frequency = calculate_frequency_shannon(blocks_str); // Use Shannon-Fano frequency
        int total_chars = text_in_file.size();
        std::vector<Block> blocks;

        for (const auto& pair : frequency){
            double prob = static_cast<double>(pair.second)/total_chars;
            blocks.emplace_back(pair.first, prob); 
        }
        
        std::sort(blocks.begin(), blocks.end(), compareBlocks);

        std::map<std::string, std::string> codes;
        generateShannonFanoCodes(blocks, codes, "");
        
        write_encoded_blocks_to_file(codes, "encoded_output_shannon.txt");
    }
    else if(change == "2"){
        std::map<std::string, std::string> reverse_codes;
        read_encoded_data(encoded_filename, reverse_codes);
    
        // Чтение двоичной последовательности из файла input2.txt
        std::string encoded_sequence = read_binary_sequence(binary_sequence_filename);
    
        // Проверка считанной последовательности
        if (encoded_sequence.empty()) {
            std::cerr << "Ошибка: считанная двоичная последовательность пуста." << std::endl;
            return 1; // Завершение программы с ошибкой
        }
    
        // Декодирование строки
        std::string decoded_string = decode(encoded_sequence, reverse_codes);
    
        // Проверка декодированной строки
        if (decoded_string.empty()) {
            std::cerr << "Ошибка: декодированная строка пуста." << std::endl;
            return 1; // Завершение программы с ошибкой
        }
    
        // Вывод декодированной строки на экран
        std::cout << "Декодированная строка:\n" << decoded_string << "\n";
    
        // Запись декодированной строки в файл
        write_decoded_string_to_file(decoded_string, output_filename);
    }
    else if (change == "3") {
        std::string text_in_file = read_text(input_filename);
        auto freq = calculate_frequency_huffman(text_in_file); // Use Huffman frequency

        HuffmanNode* root = buildHuffmanTree(freq);
        std::map<char, std::string> huffmanCode;
        printCodes(root, "", huffmanCode);

        std::cout << "\nHuffman Codes:\n";
        for (auto pair : huffmanCode) {
            std::cout << pair.first << " : " << pair.second << "\n";
        }
        write_encoded_blocks_to_file(huffmanCode, "encoded_output22.txt");
    }
    else{std::map<std::string, std::string> reverse_codes; // Измените на char для декодирования
    read_encoded_data(encoded_filename_2, reverse_codes);

    // Чтение двоичной последовательности из файла
    std::string encoded_sequence = read_binary_sequence(binary_sequence_filename_2);
    
    // Проверка считанной последовательности
    if (encoded_sequence.empty()) {
        std::cerr << "Ошибка: считанная двоичная последовательность пуста." << std::endl;
        return 1; // Завершение программы с ошибкой
    }

    // Декодирование строки с использованием обратных кодов
    std::string decoded_string;
    std::string current_code;

    for (char bit : encoded_sequence) {
        current_code += bit; // Добавляем текущий бит к коду

        // Проверяем, есть ли текущий код в обратных кодах
        if (reverse_codes.find(current_code) != reverse_codes.end()) {
            decoded_string += reverse_codes[current_code]; // Добавляем символ к результату
            current_code.clear(); // Очищаем текущий код для следующего символа
        }
    }

    // Проверка декодированной строки
    if (decoded_string.empty()) {
        std::cerr << "Ошибка: декодированная строка пуста." << std::endl;
        return 1; // Завершение программы с ошибкой
    }

    // Вывод декодированной строки на экран
    std::cout << "Декодированная строка:\n" << decoded_string << "\n";

    // Запись декодированной строки в файл
    write_decoded_string_to_file(decoded_string, output_filenameHUF);
    }
}
