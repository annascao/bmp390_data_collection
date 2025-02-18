# bmp390_data_collection

## BMP390 EXAMPLE

```c
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include "bmp390_stm32.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

SPI_HandleTypeDef hspi2;

/* USER CODE BEGIN PV */
FATFS fs;
FIL fil;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_SPI2_Init(void);
/* USER CODE BEGIN PFP */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  char string[30] = {'\0'};

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_SPI2_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  bmp390_assignI2C(&hi2c1);

  bmp390_setMode(BMP3_ENABLE, BMP3_ENABLE, BMP3_MODE_FORCED);
  bmp390_setOversampling(BMP3_OVERSAMPLING_2X , BMP3_OVERSAMPLING_2X); //pressure, temperature
  bmp390_setIIRCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp390_setOutputDataRate(BMP3_ODR_50_HZ);

  bmp3_calib_data calib = bmp390_getCalibData();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    comp_data data = bmp390_getData(&calib);
    double altitude = 44330.0 * (1.0 - pow((data.pressure/100.0) / 1013.25, 0.1903));
    printf("temp = %.5f, press = %.5f, alt = %.5f\r\n", data.temperature, data.pressure/100.0, altitude);
    HAL_Delay(500);

    // Saving to microSD card
    sprintf(string, "T: %.2f P: %.2f A: %.2f\n", data.temperature, data.pressure / 100.0, altitude);
    f_mount(&fs, "", 0);
    f_open(&fil, "test.txt", FA_OPEN_ALWAYS | FA_WRITE | FA_READ);
    f_lseek(&fil, fil.fsize);
    f_puts(string, &fil);
    f_close(&fil);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}
```