#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
using namespace std;

#define NUM_REGISTERS 20

// Define dictionary structure for memory
typedef struct {
    int registers[NUM_REGISTERS]; // Array to store register values
    int accumulator; // Accumulator register
    int mar; // Memory Address Register
    int mdr; // Memory Data Register
    int control_unit; // Control Unit register
} Memory;

// Define memory instance
Memory mem;

// Flag to pause the instruction cycle
bool pause_flag = false;

// Function to execute SET instruction
void execute_SET(const char* value_str, const char* registerName_str) {
    int value = atoi(value_str);
    int registerIndex = atoi(&registerName_str[1]) - 1; // Extract register index from register name
    
    // Set the value in the specified register
    if (registerIndex >= 0 && registerIndex < NUM_REGISTERS) {
        mem.registers[registerIndex] = value;
        
    } else {
        printf("Invalid register index.\n");
    }
}


// Function to execute ADD instruction
// Function to execute ADD instruction
// Function to execute ADD instruction
void execute_ADD(const char* registerName1_str, const char* registerName2_str, const char* registerName3_str) {
    // Determine which ADD operation to perform based on the positions of the nulls
    if (strcmp(registerName2_str, "NULL") == 0 && strcmp(registerName3_str, "NULL") == 0) {
        // ADD D1 NULL NULL
        int registerIndex1 = atoi(&registerName1_str[1]) - 1; // Extract register index from register name
        if (registerIndex1 >= 0 && registerIndex1 < NUM_REGISTERS) {
            // Direct register-to-register addition
            mem.accumulator += mem.registers[registerIndex1];
        }
    } else if (strcmp(registerName3_str, "NULL") == 0) {
        // ADD D1 D3 NULL
        int registerIndex1 = atoi(&registerName1_str[1]) - 1; // Extract register index from register name
        int registerIndex2 = atoi(&registerName2_str[1]) - 1;
        if (registerIndex1 >= 0 && registerIndex1 < NUM_REGISTERS &&
            registerIndex2 >= 0 && registerIndex2 < NUM_REGISTERS) {
            // Direct register-to-register addition
            mem.accumulator=0; // we need to restart the accumulator when two registers are summing.
            mem.accumulator += mem.registers[registerIndex1] + mem.registers[registerIndex2];
        }
    } else {
        // ADD D1 D3 D4
        int registerIndex1 = atoi(&registerName1_str[1]) - 1; // Extract register index from register name
        int registerIndex2 = atoi(&registerName2_str[1]) - 1;
        int registerIndex3 = atoi(&registerName3_str[1]) - 1;
        if (registerIndex1 >= 0 && registerIndex1 < NUM_REGISTERS &&
            registerIndex2 >= 0 && registerIndex2 < NUM_REGISTERS &&
            registerIndex3 >= 0 && registerIndex3 < NUM_REGISTERS) {
            // Memory-to-register addition
            mem.mar = mem.registers[registerIndex2]; // Set MAR to the memory address specified by D3
            mem.mdr = registerIndex2; // Load data from memory into MDR
            mem.registers[registerIndex3] = mem.registers[registerIndex1] + mem.registers[registerIndex2]; // Perform addition
        }
    }
}


// Function to execute DEC instruction
void execute_DEC(const char* registerName_str) {
    int registerIndex = atoi(&registerName_str[1]) - 1; // Extract register index from register name
    
    // Decrement the value in the specified register by 1
    if (registerIndex >= 0 && registerIndex < NUM_REGISTERS) {
        mem.registers[registerIndex]--;
    } else {
        printf("Invalid register index.\n");
    }
}

void execute_INC(const char* registerName_str) {
    int registerIndex = atoi(&registerName_str[1]) - 1; // Extract register index from register name
    
    // Decrement the value in the specified register by 1
    if (registerIndex >= 0 && registerIndex < NUM_REGISTERS) {
        mem.registers[registerIndex]++;
    } else {
        printf("Invalid register index.\n");
    }
}

// Function to execute STR instruction
void execute_STR(const char* registerIndex) {
    // Verificar si la cadena tiene al menos tres caracteres y comienza con 'D'
    if (strlen(registerIndex) >= 2 && registerIndex[0] == 'D') {
        // Extraer el número de registro de la cadena
        int index = atoi(&registerIndex[1]); // Convertir la subcadena a entero
        
        // Verificar si el índice está dentro de los límites válidos
        if (index >= 1 && index <= NUM_REGISTERS) {
            mem.registers[index - 1] = mem.accumulator; // Ajustar el índice a base 0
        } else {
            printf("Invalid register index.\n");
        }
    } else {
        printf("Invalid register format.\n");
    }
}



// Function to execute SHW instruction
void execute_SHW(const char* registerName_str) {
    int registerIndex;
    
    if (registerName_str[0] == 'D' && strlen(registerName_str) > 1) {
        registerIndex = atoi(&registerName_str[1]) - 1; // Extract register index from register name
    } else if (strcmp(registerName_str, "ACC") == 0) {
        registerIndex = NUM_REGISTERS; // Accumulator
    } else if (strcmp(registerName_str, "MAR") == 0) {
        registerIndex = NUM_REGISTERS + 1; // MAR
    } else if (strcmp(registerName_str, "MDR") == 0) {
        registerIndex = NUM_REGISTERS + 2; // MDR
    } else if (strcmp(registerName_str, "UC") == 0) {
        registerIndex = NUM_REGISTERS + 3; // Control Unit
    } else {
        printf("Invalid register name.\n");
        return;
    }

    // Show the value in the specified register or control unit
    if (registerIndex >= 0 && registerIndex < NUM_REGISTERS) {
        printf("Value in register D%d: %d\n", registerIndex + 1, mem.registers[registerIndex]);
    } else if (registerIndex == NUM_REGISTERS) {
        printf("Value in accumulator: %d\n", mem.accumulator);
    } else if (registerIndex == NUM_REGISTERS + 1) {
        printf("Value in MAR: %d\n", mem.mar);
    } else if (registerIndex == NUM_REGISTERS + 2) {
        printf("Value in MDR: %d\n", mem.mdr);
    } else if (registerIndex == NUM_REGISTERS + 3) {
        printf("Value in Control Unit: %d\n", mem.control_unit);
    } else {
        printf("Invalid register index.\n");
    }
}


// Function to execute PAUSE instruction
void execute_PAUSE() {
    // Set the pause flag to true
    pause_flag = true;
}

// Function to execute SHW instruction or pause and show register values
void execute_SHW_or_PAUSE(int registerIndex) {
    if (registerIndex >= 0 && registerIndex < NUM_REGISTERS) {
        printf("Value in register D%d: %d\n", registerIndex, mem.registers[registerIndex]);
    } else if (registerIndex == NUM_REGISTERS) {
        printf("Value in accumulator: %d\n", mem.accumulator);
    } else if (registerIndex == NUM_REGISTERS + 1) {
        printf("Value in MAR: %d\n", mem.mar);
    } else if (registerIndex == NUM_REGISTERS + 2) {
        printf("Value in MDR: %d\n", mem.mdr);
    } else if (registerIndex == NUM_REGISTERS + 3) {
        printf("Value in Control Unit: %d\n", mem.control_unit);
    } else if (registerIndex == NUM_REGISTERS + 4) {
        // Pause and show all register values
        printf("Register values:\n");
        for (int i = 0; i < NUM_REGISTERS; i++) {
            printf("Value in register D%d: %d\n", i+1, mem.registers[i]);
        }
        printf("Value in accumulator: %d\n", mem.accumulator);
        printf("Value in MAR: %d\n", mem.mar);
        printf("Value in MDR: %d\n", mem.mdr);
        printf("Value in Control Unit: %d\n", mem.control_unit);
        // Set the pause flag to true
        pause_flag = true;
    } else {
        printf("Invalid register index.\n");
    }
}


void execute_LDR(const char* registerName_str) {
    int registerIndex;
    if (registerName_str[0] == 'D' && strlen(registerName_str) > 1) {
        registerIndex = atoi(&registerName_str[1]) - 1; // Extract register index from register name
    } else {
        printf("Invalid register name.\n");
        return;
    }

    // Load the value from the specified memory address into the accumulator register
    if (registerIndex >= 0 && registerIndex < NUM_REGISTERS) {
        // Set MAR to the specified memory address
        mem.mar = registerIndex;
        // Load data from memory into MDR
        mem.mdr = mem.registers[registerIndex];
        // Load data from MDR into accumulator register
        mem.accumulator = mem.mdr;
    } else {
        printf("Invalid register index.\n");
    }
}




// Function to decode and execute instructions
void fetch_and_decode(const char* instructions[][5], int num_instructions) {
    for (int i = 0; i < num_instructions && !pause_flag; i++) {
        const char* instruction_type = instructions[i][0];
        const char* value_str = instructions[i][1]; 
        const char* registerName1_str = instructions[i][2];
        const char* registerName2_str = instructions[i][3];
        const char* registerName3_str = instructions[i][4];
        
        if (strcmp(instruction_type, "SET") == 0) {
            const char* registerName1_str = instructions[i][1]; 
            const char* value_str = instructions[i][2];
            execute_SET(value_str, registerName1_str);
        } else if (strcmp(instruction_type, "ADD") == 0) {
            const char* registerName1_str = instructions[i][1];
            const char* registerName2_str = instructions[i][2];
            const char* registerName3_str = instructions[i][3];
            execute_ADD(registerName1_str, registerName2_str, registerName3_str);
        } else if (strcmp(instruction_type, "INC") == 0) {
            const char* registerName1_str = instructions[i][1];
            execute_INC(registerName1_str);
        } else if (strcmp(instruction_type, "DEC") == 0) {
            const char* registerName1_str = instructions[i][1];
            execute_DEC(registerName1_str);
        } else if (strcmp(instruction_type, "STR") == 0) {
            const char* registerName1_str = instructions[i][1];
            execute_STR(registerName1_str);
        } else if (strcmp(instruction_type, "SHW") == 0) {
             const char* registerName1_str=instructions[i][1]; 
            execute_SHW(registerName1_str);
        } else if (strcmp(instruction_type, "PAUSE") == 0) {
            execute_SHW_or_PAUSE(NUM_REGISTERS + 4); // Show all register values
            execute_PAUSE();
        } else if (strcmp(instruction_type, "END") == 0) {
            
        }else if (strcmp(instruction_type, "LDR") == 0) {
            const char* registerName1_str = instructions[i][1];
            if (strcmp(registerName1_str, "NULL") != 0) {
                execute_LDR(registerName1_str);
            } else {
                printf("Invalid register name.\n");
            }
        } else {
            printf("Unknown instruction type.\n");
        }
    }
}




void process_file(const string& filename) {
    ifstream archivo(filename);
    vector<vector<string>> lista_de_listas;

    if (archivo.is_open()) {
        int num_instructions = 0;
        string linea;
        while (getline(archivo, linea)) {
            stringstream ss(linea);
            string elemento;
            vector<string> lista;
            while (ss >> elemento) {
                lista.push_back(elemento);
            }
            num_instructions+=1;
            lista_de_listas.push_back(lista);
        }
        archivo.close();
        const char* instructions[lista_de_listas.size()][5];
        for (size_t i = 0; i < lista_de_listas.size(); ++i) {
            for (size_t j = 0; j < lista_de_listas[i].size(); ++j) {
                instructions[i][j] = lista_de_listas[i][j].c_str();
            }
        }

        // Imprimir la lista de listas convertida
        for (size_t i = 0; i < lista_de_listas.size(); ++i) {
            cout << "{";
            for (size_t j = 0; j < 5; ++j) {
                cout << "\"" << instructions[i][j] << "\", ";
            }
            cout << "}" << endl;
        }
        fetch_and_decode(instructions, num_instructions);
    } else {
        cout << "No se pudo abrir el archivo " << filename << "." << endl;
    }
}

int main() {
    process_file("instructions.txt");
    process_file("instructions1.txt");
    process_file("instructions2.txt");
    process_file("instructions3.txt");

    return 0;
}
    // Number of
