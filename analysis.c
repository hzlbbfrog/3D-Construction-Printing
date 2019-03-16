/*********************************************************/
//@demo
//@�̼��⣺����V2.4
//@author��th
//@2016.11.30
//@for seu2016 ����ͷ������
/*********************************************************/

#include "chlib_k.h"
#include <stdlib.h>
void PIT0_CallBack(void);
void PIT1_CallBack(void);

#define STOP -1

#define UP 1
#define DOWN 0
int8_t ZDIR = STOP;
  
#define PLUS 1
#define MINUS 0
int8_t XDIR = STOP;
int8_t YDIR = STOP;

#define FOR 1
#define REV 0
int8_t MDIR = STOP;


struct{
  uint64_t X,Y,Z,M;
  uint64_t tempX,tempY,tempZ,tempM;
}halfPeriod;

struct{
  uint64_t X,Y,Z;
}stepRemain;

void SetDir(int8_t X, int8_t Y, int8_t Z, int8_t M){
  XDIR = X, YDIR = Y, ZDIR = Z; MDIR = M;
    //Z����
  if(ZDIR != STOP){
    PEout(4) = ZDIR;         //DIR

    PDout(6) = ZDIR;         //DIR

    PBout(11) = ZDIR;         //DIR
  }

    //X����        ��X��ϳ���
  if(XDIR != STOP)
    PBout(23) = XDIR;

  //Y����
  if(YDIR != STOP)
    PCout(9) = YDIR;
  
  //���ϵ��
  if(MDIR != STOP)
    PCout(17) = MDIR;
}

void SetXYZDir(int8_t X, int8_t Y, int8_t Z) { SetDir(X,Y,Z,MDIR); }

void SetHalfPeriod(uint64_t X,uint64_t Y,uint64_t Z,uint64_t M){
  halfPeriod.X = X;
  halfPeriod.Y = Y;
  halfPeriod.Z = Z;
  halfPeriod.M = M;
  
  halfPeriod.tempX = 0;
  halfPeriod.tempY = 0;
  halfPeriod.tempZ = 0;
  halfPeriod.tempM = 0;
}
void SetStepRemain(int64_t X,int64_t Y,int64_t Z){
  stepRemain.X = abs(X);
  stepRemain.Y = abs(Y);
  stepRemain.Z = abs(Z);
  SetXYZDir(X>=0 ? PLUS:MINUS,Y>=0 ? PLUS:MINUS,Z>=0 ? PLUS:MINUS);
}

bool IsActionFinesed(){return (stepRemain.X == 0) && (stepRemain.Y == 0) && (stepRemain.Z == 0);}
void Mpulse(){
  static bool mp = 0;
  mp ^= 1;
  if(MDIR != STOP)
    PCout(13) = mp;
}

void Zpulse(){
  static bool zp = 0;
  if(stepRemain.Z == 0) return;
  zp ^= 1;
  
  if(ZDIR!=STOP){
      
    PEout(0) = zp;
    
    PDout(2) = zp;
    
    PBout(2) = zp;
  }
  if(zp && stepRemain.Z > 0) stepRemain.Z--;
}

void Xpulse(){
  static bool xp = 0;
  if(stepRemain.X == 0) return;
  xp ^= 1;
  if(XDIR != STOP)
    PBout(19) = xp;
  
  if(xp && stepRemain.X > 0) stepRemain.X--;
}

void Ypulse(){
  static bool yp = 0;
  if(stepRemain.Y == 0) return;
  yp ^= 1;
  if(YDIR != STOP)
    PCout(5) = yp;
  
  if(yp && stepRemain.Y > 0) stepRemain.Y--;

}

void Init(){
  SetDir(STOP,STOP,STOP,FOR);
  
  PEout(0) = 0;
  
  PDout(2) = 0;
  
  PBout(2) = 0;
  
  PCout(5) = 0;

  PBout(19) = 0;

  SetHalfPeriod(1,1,1,1);  
  stepRemain.X = 0;
  stepRemain.Y = 0;
  stepRemain.Z = 0;
}
int main()
{
  DisableInterrupts;//��ʼ��֮ǰ�ȹص������ж�

    DelayInit();
  //LED
  GPIO_QuickInit(HW_GPIOA, 5, kGPIO_Mode_OPP);
  PAout(5) = 1;

  GPIO_QuickInit(HW_GPIOC, 3, kGPIO_Mode_OPP);
  GPIO_QuickInit(HW_GPIOB, 23, kGPIO_Mode_OPP);
  GPIO_QuickInit(HW_GPIOB, 19, kGPIO_Mode_OPP);
  GPIO_QuickInit(HW_GPIOB, 11, kGPIO_Mode_OPP);
  GPIO_QuickInit(HW_GPIOB, 2, kGPIO_Mode_OPP);
  
  GPIO_QuickInit(HW_GPIOD, 6, kGPIO_Mode_OPP);
  GPIO_QuickInit(HW_GPIOD, 2, kGPIO_Mode_OPP);
  GPIO_QuickInit(HW_GPIOC, 17, kGPIO_Mode_OPP);
  GPIO_QuickInit(HW_GPIOC,13, kGPIO_Mode_OPP);
  GPIO_QuickInit(HW_GPIOC, 9, kGPIO_Mode_OPP);
  GPIO_QuickInit(HW_GPIOC, 5, kGPIO_Mode_OPP);
  
  GPIO_QuickInit(HW_GPIOE, 0, kGPIO_Mode_OPP);
  GPIO_QuickInit(HW_GPIOE, 4, kGPIO_Mode_OPP);

  Init();

  PCout(3) = 0;
  
  //ע���ж�
  PIT_QuickInit(HW_PIT_CH0, 1500000); // ��ʼ����ʱ��������20ms 
  PIT_ITDMAConfig(HW_PIT_CH0, kPIT_IT_TOF,ENABLE);// ����ģ��0ͨ���ж� 
  PIT_CallbackInstall(HW_PIT_CH0, PIT0_CallBack); // ע��ص����� 
  PIT_QuickInit(HW_PIT_CH1, 50); // ��ʼ����ʱ��������20ms 
  PIT_ITDMAConfig(HW_PIT_CH1, kPIT_IT_TOF,ENABLE);// ����ģ��0ͨ���ж� 
  PIT_CallbackInstall(HW_PIT_CH1, PIT1_CallBack); // ע��ص����� 
  EnableInterrupts;

  SetHalfPeriod(10,10,2,1);
 /*     SetStepRemain(0,0,-640000);
      while(!IsActionFinesed());
      return 0;*/
  int count = 0;
  while(1){
      /*  SetDir(STOP,STOP,STOP,REV);

      SetStepRemain(6400*2,0,0);
      while(!IsActionFinesed());
      
      SetStepRemain(0,6400*2,0);
      while(!IsActionFinesed());
      
      SetStepRemain(-6400*2,0,0);
      while(!IsActionFinesed());
      
      SetStepRemain(0,-6400*2,0);
      while(!IsActionFinesed());*/
      
      SetDir(STOP,STOP,STOP,STOP);
      SetStepRemain(0,0,-15360);
      while(!IsActionFinesed());
      
      
  }
    
}

void PIT0_CallBack(void) {
     PAout(5) ^= 1;

}
void PIT1_CallBack(void) {
  if (halfPeriod.tempX == 0)
    Xpulse();
  if (halfPeriod.tempY == 0)
    Ypulse();
  if (halfPeriod.tempZ == 0)
    Zpulse();
  if (halfPeriod.tempM == 0)
    Mpulse();
  
  
  halfPeriod.tempX = (halfPeriod.tempX + 1) % halfPeriod.X;
  halfPeriod.tempY = (halfPeriod.tempY + 1) % halfPeriod.Y;
  halfPeriod.tempZ = (halfPeriod.tempZ + 1) % halfPeriod.Z;
  halfPeriod.tempM = (halfPeriod.tempM + 1) % halfPeriod.M;
  
}

#include <string.h>

void Analyze(char str[]) {
	char *command = strtok(str, " ");
	char *subCommand;

	/*=======================================G0ָ��=======================================*/
	if (strcmp(command, "G0")) {
		float X = -1, Y = -1, Z = -1;					//G0����ֻ��X��Y��E������ָ��
		while ((subCommand = strtok(NULL, subCommand)) != NULL) {
			switch (subCommand[0]) {
			case 'X':
				sscanf(subCommand + 1, "%f", &X);		//�ַ���ת��Ϊ���������X
				break;
			case 'Y':
				sscanf(subCommand + 1, "%f", &Y);		//�ַ���ת��Ϊ���������Y
				break;
			case 'E':
				sscanf(subCommand + 1, "%f", &Z);		//�ַ���ת��Ϊ���������Z
				break;
			}
		}
		MoveTo(X, Y, Z);								//��ĳ����ָ��δ����ֵ��ֵΪ-1��������Ķ�Ӧ״̬
	}
	/*=======================================G1ָ��=======================================*/
	else if (strcmp(command, "G1")) {
		float X = -1, Y = -1, Z = -1, F = -1;			        //G00������X��Y��E��F������ָ��
		while ((subCommand = strtok(NULL, subCommand)) != NULL) {
			switch (subCommand[0]) {
			case 'X':
				sscanf(subCommand + 1, "%f", &X);		//�ַ���ת��Ϊ���������X
				break;
			case 'Y':
				sscanf(subCommand + 1, "%f", &Y);		//�ַ���ת��Ϊ���������Y
				break;
			case 'E':
				sscanf(subCommand + 1, "%f", &Z);		//�ַ���ת��Ϊ���������Z
				break;
			case 'F':
				sscanf(subCommand + 1, "%f", &F);		//�ַ���ת��Ϊ���������E
				break;
			}
		}
		MoveTo(X, Y, Z, F);								//��ĳ����ָ��δ����ֵ��ֵΪ-1��������Ķ�Ӧ״̬
	}
	/*=======================================G2ָ��=======================================*/
	else if (strcmp(command, "G2")) {
		float X = -1, Y = -1, I = -1, J = -1;			        //G00������X��Y��I��J������ָ��
		while ((subCommand = strtok(NULL, subCommand)) != NULL) {
			switch (subCommand[0]) {
			case 'X':
				sscanf(subCommand + 1, "%f", &X);		//�ַ���ת��Ϊ���������X
				break;
			case 'Y':
				sscanf(subCommand + 1, "%f", &Y);		//�ַ���ת��Ϊ���������Y
				break;
			case 'I':
				sscanf(subCommand + 1, "%f", &I);		//�ַ���ת��Ϊ���������I
				break;
			case 'J':
				sscanf(subCommand + 1, "%f", &J);		//�ַ���ת��Ϊ���������J
				break;
			}
		}
		MoveCircle(X, Y, I, J);								//��ĳ����ָ��δ����ֵ��ֵΪ-1��������Ķ�Ӧ״̬
	}
	/*=======================================G29ָ��=======================================*/
	else if (strcmp(command, "G29")) {                                      //����֧�ֵ�ָ�ֱ�ӷ���
		return;
	}
	/*=======================================G30ָ��=======================================*/
	else if (strcmp(command, "G30")) {                                      //����֧�ֵ�ָ�ֱ�ӷ���
		return;
	}
	/*=======================================G31ָ��=======================================*/
	else if (strcmp(command, "G31")) {                                      //����֧�ֵ�ָ�ֱ�ӷ���
		return;
	}
	/*=======================================G32ָ��=======================================*/
	else if (strcmp(command, "G32")) {                                      //����֧�ֵ�ָ�ֱ�ӷ���
		return;
	}
	/*=======================================G90ָ��=======================================*/
	else if (strcmp(command, "G90")) {                                      //����֧�ֵ�ָ�ֱ�ӷ���
		return;
	}
	/*=======================================G91ָ��=======================================*/
	else if (strcmp(command, "G91")) {                                      //����֧�ֵ�ָ�ֱ�ӷ���
		return;
	}
	else {
		if (command[0]=='M'){
	/*=======================================Mָ��=======================================*/		
			int len = strlen(command);
			int num = 0;
			int large=1;
		for (int i = len-1;i >= 1;i--){
			int k = command[i]-48;
			num=num+k*large;
			large=large*10;
		}
		if (num>=21)&&(num<=25){											    //����֧�ֵ�ָ�ֱ�ӷ���
			return;                                                    
		}
		else if (num>=104)&&(num<=190){											//����֧�ֵ�ָ�ֱ�ӷ���
			return;                                                    
		}	
		else if (num>=201)&&(num<=250){											//����֧�ֵ�ָ�ֱ�ӷ���
			return;                                                    
		}	
		else if (num>=300)&&(num<=304){											//����֧�ֵ�ָ�ֱ�ӷ���
			return;                                                    
		}
		else if (num>=500)&&(num<=503){											//����֧�ֵ�ָ�ֱ�ӷ���
			return;                                                    
		}
	}
	


}