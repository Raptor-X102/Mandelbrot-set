# Отчет о выполнении работы "Множество Мандельброта"

Mandelbrot set: space theme
![Mandelbrot_fullsize](Graph_Mandelbrot/Mandelbrot_fullsize.png)
![Mandelbrot_zoomed](Graph_Mandelbrot/Mandelbrot_zoomed.png)
## Цель:
Написать в трех версиях программу, которая вычисляет точки множества Мандельброта, на основе вычислений заносит цвет точки в массив пикселей и выводит на экран. Исследовать различные способы оптимизаций программы.

## Принцип измерения скорости

Основные замеры происходят в вызове Master_test. В ней для каждой версии запускается проход функции с "прологом" и "эпилогом" функции (здесь эта версия называется "void"). Это делается больше для отчетности, потому что на этот вызов тратится примерно в 10^6 раз меньше времени, чем на основную функцию, что в целом ника не влияет на средние количество тактов на вызов функции. Далее делается замер самой функции, а после вычисление всех показателей.

<details>
<summary>Пример функции версии void</summary>

```c
void Calc_func_basic_void(uint32_t* pixels, Frame_movement movement) {

    __asm__ volatile (
        "" : :
        "r"(pixels),
        "r"(movement.x_offset),
        "r"(movement.scale)
    );
    volatile real_t dx = 2.0f / (X_Pixels_Size * movement.scale);
    volatile real_t dy = 2.0f / (X_Pixels_Size * movement.scale);
    volatile real_t tem_x0 = X_0 + movement.x_offset;
    return;
}
```
</details>

<details>
<summary>Пример вызова</summary>

```c
void Master_test(uint32_t* pixels, Frame_movement movement) {

    time_t now = time(NULL);
    struct tm *curr_time = localtime(&now);
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", curr_time);
    printf("#Test session\n");
    printf("```\n");
    printf("Test start\nTime: %s\n", buffer);
    printf("Resolution: %dx%d\n", X_Pixels_Size, Y_Pixels_Size);
    printf("Center: %.3lf, %.3lf\n", X_0, Y_0);
    printf("Max_iter: %d\n", Max_iter);
    printf("```\n");

    printf("## Calc_func_basic\n");
    Test_data test_data_void = Single_test(Calc_func_basic_void, pixels, movement,
                                           Test_num_of_cycles_void, "### void");
    Test_data test_data = Single_test(Calc_func_basic, pixels, movement,
                                      Test_num_of_cycles_basic, "### full version");
    Test_accuracy(&test_data, &test_data_void, "### Calc_func_basic real data");

    ....
}
```
</details>

Замер времени происходит двумя способами: через библиотеку time.h и rdtscp. Данные замеров заносятся в структуру Test_data.

<details>
<summary>Показать функцию замера</summary>

```c
Test_data Single_test(Calc_func_t Calc_func, uint32_t* pixels, Frame_movement movement,
                      int test_num_of_cycles, const char* label) {

    Test_data test_data = {};

    test_data.frequency_start = Frequency_counter();
    test_data.timeh_t1 = clock();
    test_data.start_cycles = Rdtscp();

    for(int i = 0; i < test_num_of_cycles; i++)
        Calc_func(pixels, movement);

    test_data.end_cycles = Rdtscp();
    test_data.timeh_t2 = clock();
    test_data.frequency_end = Frequency_counter();
    Print_test_data(&test_data, test_num_of_cycles, label);

    test_data.test_num_of_cycles = test_num_of_cycles;
    return test_data;
}
```
</details>

Мы используем rdtscp, а не rdtsc чтобы избежать "out of order" выполнения программы для точного замера цикла. Инструкция cpuid нужна для этого же.

<details>
<summary>Показать функцию замера</summary>

```c
uint64_t Rdtscp() {

    uint32_t lo, hi;
    __asm__ volatile (

        "rdtscp\n\t"
        "mov %%eax, %0\n\t"
        "mov %%edx, %1\n\t"
        "cpuid\n\t"
        : "=r"(lo), "=r"(hi)
        :
        : "%rax", "%rbx", "%rcx", "%rdx"
    );
    return ((uint64_t)hi << 32) | lo;
}
```
</details>

## Что будем мерить
Мы будем мерить именно количество тактов, потому что именно это независимая от частоты величина. Мы пытались вычислить текущую частоту двумя способами, через TSP (Time stamp pointer) с частотой счетчика и реальную. Первый способ всегда выдавал 3 ГГц, потому что это ненастоящая частота, а второй выдавал сверхбольшие значения из-за того, что высчитывалась частота на всех потоках. Поэтому мы отказались от этих способов, несмотря на то, что второй можно было довести до конца. Однако как это влияло бы на многопоточность мы не знаем. Частота в экспериментальных сериях замерялась по первому методу, поэтому можно оценить приближенное значение FPS.

## main
Здесь создается структура, отвечающая за передвижение картинки, и выделяется память под массив пикселей. Происходит обработка флагов, на основе которых запускается нужная версия. Основные флаги:
1. "--graphics" и "--no-graphics". Отвечают за графический режим вывода множества Мандельброта на экран и за тестирующую версию.
2. -D ("BASIC"/"ARRAYS"/"INTRINSICS"/"MASTER_TEST"). Последний вызывает основной тест, на основе которого далее будет рассчитываться ускорение.
3. -D ("__M_128"/"__M_256"). Указывает тип регистра. (только с "-D INTRINSICS")
4. "--ps"/"--pd". Указывает точность (только с "-D INTRINSICS")
5. "--cycles num_of_cycles". Вместо num_of_cycles прописывается количество циклов для тестирующей версии (только с "--no-graphics").

## Краткое описание программы
В данном проекте написано 6 версий:
1. Простейшая
2. С циклами и массивами
3. xmm и single-precision
4. xmm и double-precision
5. ymm и single-precision
6. ymm и double-precision

## Основные версии программы
### 1. Простейшая
В двойном цикле высчитывает последовательно каждую точку. Используемый тип данных - float (real_t).

<details>
<summary>Показать Calc_func_basic</summary>

```c
for (int y_pixels = 0; y_pixels < Y_Pixels_Size; y_pixels++){

        real_t current_y = movement.y_0 + y_pixels * dy;
        for (int x_pixels = 0; x_pixels < X_Pixels_Size; x_pixels++) {

            real_t start_x = movement.x_0 + x_pixels * dx;
            real_t x = start_x, y = current_y;
            real_t const_x = start_x, const_y = current_y;
            int num_of_iter = 0;
            for (; num_of_iter < Max_iter; num_of_iter++) {     // Проверяется только 1 точка за итерацию

                real_t sqr_x = x * x, sqr_y = y * y, mul_xy = x * y;
                real_t sqr_r = sqr_x + sqr_y;
                if (sqr_r >= Max_sqr_r)
                    break;

                x = sqr_x - sqr_y + const_x;
                y = mul_xy + mul_xy + const_y;
            }

            if (num_of_iter == 256)                                     // Заносим цвет на основе итерации вылета
                pixels[y_pixels * X_Pixels_Size + x_pixels] = 255;

            else
                pixels[y_pixels * X_Pixels_Size + x_pixels] = (num_of_iter << 24)                         |
                                                              (num_of_iter+num_of_iter << 16)             |
                                                              (num_of_iter+num_of_iter+num_of_iter << 8)  |
                                                               255;
        }
    }
```
</details>

Результат
| Версия функции     	| Количество тактов, 10^8 	| Среднее значение FPS 	|
|--------------------	|-------------------------	|----------------------	|
| Простейшая (basic) 	| 3,5                     	| 8,6                  	|

### 2. На основе циклов и массивов
Суть этой версии в том, чтобы при компиляции с ключом -O3 компилятор оптимизировал сам код, заменив циклы и работу с массивами на работу с xmm/ymm-регистрами, что значительно увеличит скорость. По сути, мы делаем векторизацию кода.

<details>
<summary>Показать Calc_func_arrays</summary>

```c
for (int y_pixels = 0; y_pixels < Y_Pixels_Size; y_pixels++) {

    real_t vec_r_t_x     [Vector_length] = {};
    real_t vec_r_t_y     [Vector_length] = {};
    real_t vec_r_t_sqr_x [Vector_length] = {};
    real_t vec_r_t_sqr_y [Vector_length] = {};      // массивы, содержащие 4 или 8 элементов сразу
    real_t vec_r_t_mul_xy[Vector_length] = {};
    real_t vec_r_t_sqr_r [Vector_length] = {};
    real_t const_x       [Vector_length] = {};
    real_t const_y       [Vector_length] = {};

    real_t current_y = movement.y_0 + y_pixels * dy;
    for (int x_pixels = 0; x_pixels < X_Pixels_Size; x_pixels += Vector_length) {

        mask_t vec_status = -1;
        uint32_t vec_colour[Vector_length] = {};
        real_t start_x = movement.x_0 + x_pixels * dx;
        for (int i = 0; i < Vector_length; i++) {

            vec_r_t_x[i] = start_x + i * dx;
            vec_r_t_y[i] = current_y;
            const_x[i] = start_x + i * dx;
            const_y[i] = current_y;
        }

        for (int i = 0; i < Vector_length; i++) {       // высчитывается сразу несколько значений точек

            vec_r_t_sqr_x [i] = vec_r_t_x    [i] * vec_r_t_x    [i];
            vec_r_t_sqr_y [i] = vec_r_t_y    [i] * vec_r_t_y    [i];
            vec_r_t_mul_xy[i] = vec_r_t_x    [i] * vec_r_t_y    [i];
            vec_r_t_sqr_r [i] = vec_r_t_sqr_x[i] + vec_r_t_sqr_y[i];
        }

        int num_of_iter = 0;
        for (; num_of_iter < Max_iter; num_of_iter++) {

            for (int i = 0; i < Vector_length; i++) {

                if (vec_r_t_sqr_r[i] >= Max_sqr_r) {        // сравнивается сразу несколько точек в 1 итерации

                    vec_status &= ~mask_array[i];
                }

                vec_r_t_x     [i] = vec_r_t_sqr_x [i] - vec_r_t_sqr_y [i] + const_x[i];
                vec_r_t_y     [i] = vec_r_t_mul_xy[i] + vec_r_t_mul_xy[i] + const_y[i];
                vec_r_t_sqr_x [i] = vec_r_t_x     [i] * vec_r_t_x     [i];
                vec_r_t_sqr_y [i] = vec_r_t_y     [i] * vec_r_t_y     [i];
                vec_r_t_mul_xy[i] = vec_r_t_x     [i] * vec_r_t_y     [i];
                vec_r_t_sqr_r [i] = vec_r_t_sqr_x [i] + vec_r_t_sqr_y [i];
            }

            if (!vec_status)
                break;

            for (int i = 0; i < Vector_length; i++) {

                mask_t mask_cast = vec_status >> (8 * i);
                vec_colour[i] += (mask_cast & 0xFF) == 0xFF;
            }
        }

        for (int i = 0; i < Vector_length; i++) {

            if ((vec_status & mask_array[i]))
                pixels[y_pixels * X_Pixels_Size + x_pixels + i] = Max_iter;

            else
                pixels[y_pixels * X_Pixels_Size + x_pixels + i] = (vec_colour[i] << 24)                                |
                                                                    (vec_colour[i] + vec_colour[i] << 16)                |
                                                                    (vec_colour[i] + vec_colour[i] + vec_colour[i] << 8) |
                                                                    255;
        }
    }
}
```
</details>

Результаты
| Версия функции           	| Количество тактов, 10^8 	| Среднее значение FPS 	|
|--------------------------	|-------------------------	|----------------------	|
| Массивы и циклы (arrays) 	| 1,3                     	| 22,6                 	|

### 3. С помощью intrinsics
SIMD осуществляется за счет интринсиков - функций, соответствующие машинным инструкциям процессора.

<details>
<summary>Показать Calc_func_intrinsics128_ps</summary>

```c
for (int y_pixels = 0; y_pixels < Y_Pixels_Size; y_pixels++) {

    __m128 vec_r_t_x;
    __m128 vec_r_t_y;
    __m128 vec_r_t_sqr_x;
    __m128 vec_r_t_sqr_y;
    __m128 vec_r_t_mul_xy;      // вместо массивов - инструкции для работы с регистрами напрямую
    __m128 vec_r_t_sqr_r;
    __m128 const_x;
    __m128 const_y;
    __m128 vec_status;
    alignas(16) uint32_t vec_colour_32[4] = {};
    alignas(16) uint32_t vec_status_32[4] = {};

    float current_y = movement.y_0 + y_pixels * dy;
    for (int x_pixels = 0; x_pixels < X_Pixels_Size; x_pixels += Vector_length) {

        float start_x = movement.x_0 + x_pixels * dx;

        vec_r_t_x = MM_SET_PS(start_x, dx);
        vec_r_t_y = _mm_set_ps1(current_y);
        const_x = vec_r_t_x;
        const_y = vec_r_t_y;

        vec_r_t_sqr_x  = _mm_mul_ps(vec_r_t_x, vec_r_t_x);
        vec_r_t_sqr_y  = _mm_mul_ps(vec_r_t_y, vec_r_t_y);
        vec_r_t_mul_xy = _mm_mul_ps(vec_r_t_x, vec_r_t_y);
        vec_r_t_sqr_r  = _mm_add_ps(vec_r_t_sqr_x, vec_r_t_sqr_y);

        int num_of_iter = 0;
        __m128i iterations = _mm_set_epi64x(0, 0);
        for (; num_of_iter < Max_iter; num_of_iter++) {

            vec_status = _mm_cmple_ps(vec_r_t_sqr_r, max_sqr_r_vec);
            if (!_mm_movemask_epi8(_mm_castps_si128(vec_status)))
                break;
            iterations = _mm_sub_epi32(iterations, _mm_castps_si128(vec_status));

            vec_r_t_x      = _mm_add_ps(_mm_sub_ps(vec_r_t_sqr_x, vec_r_t_sqr_y), const_x);
            vec_r_t_y      = _mm_add_ps(_mm_add_ps(vec_r_t_mul_xy, vec_r_t_mul_xy), const_y);
            vec_r_t_sqr_x  = _mm_mul_ps(vec_r_t_x, vec_r_t_x);
            vec_r_t_sqr_y  = _mm_mul_ps(vec_r_t_y, vec_r_t_y);
            vec_r_t_mul_xy = _mm_mul_ps(vec_r_t_x, vec_r_t_y);
            vec_r_t_sqr_r  = _mm_add_ps(vec_r_t_sqr_x, vec_r_t_sqr_y);
        }

        _mm_store_si128((__m128i*) vec_colour_32, iterations);
        _mm_store_si128((__m128i*) vec_status_32, _mm_castps_si128(vec_status));
        for (int i = 0; i < Vector_length; i++) {

            if (vec_status_32[i])
                pixels[y_pixels * X_Pixels_Size + x_pixels + i] = Max_iter;

            else
                pixels[y_pixels * X_Pixels_Size + x_pixels + i] =   (vec_status_32[i] << 24) |
                                                                    (vec_status_32[i] + vec_colour_32[i] << 16) |
                                                                    (vec_status_32[i] + vec_colour_32[i] + vec_colour_32[i] << 8) |
                                                                    255;
        }
    }
}
```
</details>

Результаты
Intrinsics с типом данных float
| Версия функции   	| Количество тактов, 10^7 	| Среднее значение FPS 	|
|------------------	|-------------------------	|----------------------	|
| Intrinsics128_ps 	| 9,5                     	| 31,5                 	|
| Intrinsics256_ps 	| 4,8                     	| 62,9                 	|

Intrinsics с типом данных double
| Версия функции   	| Количество тактов, 10^8 	| Среднее значение FPS 	|
|------------------	|-------------------------	|----------------------	|
| Intrinsics128_pd 	| 1,4                     	| 12,2                 	|
| Intrinsics256_pd 	| 0,9                     	| 32,7                 	|

## Многопоточность

Условная компиляция запускается через
```
make PARALLEL=yes
```

Здесь используется _Pragma("omp parallel for schedule(dynamic, 2)").
dynamic отвечает за динамическое распределение, т.е. потоки берут задачи по мере освобождения. 2 - количество итераций каждому потоку.

## Результаты

Среднее количество тиков на 1 вызов

Тип данных float
|                         	| Basic 	| Arrays 	| Intrinsics128_ps 	| Intrinsics256_ps 	|
|-------------------------	|-------	|--------	|------------------	|------------------	|
| Количество тактов, 10^6 	| 29,7      | 22,5  	| 9,7              	| 5,6              	|
| Среднее значение FPS    	| 77,4  	| 133,5  	| 309,31           	| 538,1            	|

Тип данных double
|                         	| Intrinsics128_pd 	| Intrinsics256_pd 	|
|-------------------------	|------------------	|------------------	|
| Количество тактов, 10^6 	| 13,9             	| 10,6             	|
| Среднее значение FPS    	| 121,6            	| 284,1            	|

Убрав из расчета несколько значений, в которых количество тиков значительно превосходит средние (скорее всего это связано с тем, что в этот момент параллельно протекали еще какие-то независимые процессы), мы рассчитали случайную погрешность. Погрешности измерения составляют не более процента. Данные находятся в exel таблице.

## Итоговые результаты и множители ускорения

Тип данных float
|                   	| Basic 	| Arrays 	| Intrinsics128_ps 	| Intrinsics256_ps 	|
|-------------------	|-------	|--------	|------------------	|------------------	|
| Parallel Disabled 	| 1,0  	    | 2,6   	| 3,7             	| 7,4             	|
| Parallel Enabled  	| 11,8 	    | 15,6  	| 36,2            	| 63,0            	|

Тип данных double
|                   	| Intrinsics128_pd 	| Intrinsics256_pd 	|
|-------------------	|------------------	|------------------	|
| Parallel Disabled 	| 2,6              	| 3,8              	|
| Parallel Enabled  	| 25,3             	| 33,2             	|

А также относительный прирост скорости из-за многопоточности

|              	| Basic    	| Arrays  	| Intrinsics128_ps 	| Intrinsics128_pd 	| Intrinsics256_ps 	| Intrinsics256_pd 	|
|--------------	|----------	|---------	|------------------	|------------------	|------------------	|------------------	|
| В кол-во раз 	| 11,8    	| 5,9    	| 9,8             	| 9,9             	| 8,6             	| 8,7             	|

![Тип данных float](histograms/hystogram1.png)
![Тип данных double](histograms/hystogram3.png)
![Тип данных float + parallel](histograms/hystogram2.png)
![Тип данных double + parallel](histograms/hystogram4.png)

# Выводы
В данной работе были написано 6 различных версий рассчета множества Мандельброта. Была исследована зависимость ускорения программы от типа оптимизации. На основе этого сделаны сравнительные таблицы и гистограммы.


# Характеристики
| Устройство               	| Huawei Matebook D 16                                                	|
|:-------------------------:|:---------------------------------------------------------------------:|
| OS                       	| Windows 11 Домашняя                                                 	|
| CPU                      	| 13th Gen Intel(R) Core(TM) i9-13900H                                	|
| Объем оперативной памяти 	| 16,0 ГБ                                                             	|
| Тип системы              	| 64-разрядная операционная система, процессор x64                    	|
| Компилятор               	| x86_64-w64-mingw32-g++.exe<br>(Rev3, Built by MSYS2 project) 14.2.0 	|
