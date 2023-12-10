//with convertation
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <conio.h>
#include <Windows.h>
#include <math.h>
#include <stdlib.h>

#define RED "\x1b[31m"
#define BLUE "\x1b[36m"
#define RESET "\x1b[0m"
#define GREEN "\033[0;32m"
#define ESC 27

#define MAX 1e6
#define MIN 1e-6
#define PI 3.14159265359

typedef struct {
    double real;
    double imag;
} compl_res;

void print_greeting() {
    printf(BLUE"Hello! This is a program for calculating complex resistance of circuit depending on the current frequency\n"RESET);
    printf(BLUE"The maximum value you can enter is %.e, the minimum value is %.e\n"RESET, MAX, MIN);
}

bool is_esc() {
    printf(BLUE"Press ESC key to exit, or any other key to try again: "RESET);
    char escChoice = _getch();
    printf("\n");
    system("cls");
    return escChoice == ESC;
}

bool is_input_valid(void* inp_ptr, const char* format) {
    int start_space = 0;
    int newline = 0;
    int result = scanf(format, &start_space, inp_ptr, &newline);
    rewind(stdin);
    if (start_space == 0 && result == 2 && newline == '\n') {
        return true;
    }
    printf(RED"Error! Invalid input format!\n\n"RESET);
    return false;
}

bool is_choice_valid(int num) {
    if (!(num >= 1 && num <= 4)) {
        printf(RED"Invalid choice!\n"RESET);
        return false;
    }
    return true;
}

bool is_data_valid(double num) {
    if (!(num >= MIN && num <= MAX)) {
        printf(RED"Invalid value!\n"RESET);
        return false;
    }
    return true;
}

void take_choice(int* num_ptr) {
    do {
        printf(BLUE"Choose the circuit (1 - 4): "RESET);
    } while (!is_input_valid(num_ptr, " %n%d%c") || !is_choice_valid(*num_ptr));
}

void take_data(double* l, double* c, double* r1, double* r2, double* f_min, double* f_max, double* step, int ch) {
    bool is_interval_valid = false;
    do {
        printf(BLUE"Enter L (мГн): "RESET);
    } while (!is_input_valid(l, " %n%lf%c") || !is_data_valid(*l));
    do {
        printf(BLUE"Enter C (мкФ): "RESET);
    } while (!is_input_valid(c, " %n%lf%c") || !is_data_valid(*c));
    do {
        printf(BLUE"Enter R1 (Ом): "RESET);
    } while (!is_input_valid(r1, " %n%lf%c") || !is_data_valid(*r1));
    if (ch == 3 || ch == 4) {
        do {
            printf(BLUE"Enter R2 (Ом): "RESET);
        } while (!is_input_valid(r2, " %n%lf%c") || !is_data_valid(*r2));
    }

    do {
        do {
            printf(BLUE"Enter F min (Гц): "RESET);
        } while (!is_input_valid(f_min, " %n%lf%c") || !is_data_valid(*f_min));
        do {
            printf(BLUE"Enter F max (Гц): "RESET);
        } while (!is_input_valid(f_max, " %n%lf%c") || !is_data_valid(*f_max));
        if (*f_min < *f_max) {
            is_interval_valid = true;
        }
        else {
            printf(RED"Error! F min must be less than F max\n"RESET);
        }
    } while (!is_interval_valid);
    do {
        printf(BLUE"Enter step: "RESET);
    } while (!is_input_valid(step, " %n%lf%c") || !is_data_valid(*step));
}

double convert(double num, double power) {
    return num * pow(10, -power);
}

compl_res calc(double R1, double R2, double L, double C, double fr, int ch) {
    compl_res resistance;
    double w = 2 * PI * fr;
    double a = 0, b = 0, c = 0, d = 0, re = 0, im = 0;
    switch (ch) {
    case 1:
        a = L / C;
        b = -R1 / (w * C);
        c = R1;
        d = (w * L) - 1 / (w * C);
        break;
    case 2:
        a = L / C;
        b = R1 / (w * C);
        c = R1;
        d = (w * L) - 1 / (w * C);
        break;
    case 3:
        a = R1 * R2;
        b = R1 * ((w * L) - 1 / (w * C));
        c = R1 + R2;
        d = (w * L) - 1 / (w * C);
        break;
    case 4:
        a = (R1 * R2) + (L / C);
        b = (w * L * R1) - R2 / (w * C);
        c = R1 + R2;
        d = (w * L) - 1 / (w * C);
        break;
    default:
        printf(RED"An error happened!\n"RESET);
        break;
    }
    re = (a * c + b * d) / (c * c + d * d);
    im = (b * c - a * d) / (c * c + d * d);

    resistance.real = re;
    resistance.imag = im;
    return resistance;
}

double frequency(double L, double C) {
    double denom = 2 * PI * sqrt(L * C);
    return 1 / denom;
}

int show_result(double R1, double R2, double L, double C, double f_min, double f_max, double step, int ch) {
    double induct = convert(L, 3), capacity = convert(C, 6);
    double fr = frequency(induct, capacity);
    printf("\nResonant frequency = %e\n\n", fr);
    printf("Frequency\t\tResistance\n\n");
    double diff = (f_max - f_min) / step, add = 0;
    if (diff - (int)diff == 0) {
        add = 1.0;
    }
    else {
        add = 2.0;
    }
    int size = (int)(diff)+add;

    double* freq = (double*)calloc(size, sizeof(double));

    if (freq == NULL) {
        return -1;
    }

    for (int i = 0; i < size; ++i) {
        freq[i] = f_min + i * step;
        if (diff / step != 0 && i == size - 1) {
            freq[i] = f_max;
        }
    }

    for (int i = 0; i < size; ++i) {
        compl_res result = calc(R1, R2, induct, capacity, freq[i], ch);
        printf("%lf\t\t%e + %e i\n", freq[i], result.real, result.imag);
        if (i % 15 == 0 && i != 0 && i != (size - 1)) {
            printf(BLUE"Press any key to continue\n"RESET);
            _getch();
        }
    }
    free(freq);
    return 0;
}

int main() {
    int circuit = 0;
    double L = 0, C = 0, R1 = 0, R2 = 0, F_min = 0, F_max = 0, step = 0;
    do {
        print_greeting();
        take_choice(&circuit);
        take_data(&L, &C, &R1, &R2, &F_min, &F_max, &step, circuit);
        if (show_result(R1, R2, L, C, F_min, F_max, step, circuit) == -1) {
            printf(RED"An error happened! Sorry!\n"RESET);
        }
    } while (!is_esc());
    return 0;
}
