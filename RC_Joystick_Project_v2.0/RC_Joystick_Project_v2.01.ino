// RC Joystick Project Version 2.01
/*Seçkin Albamya (TA3UAS) tarafından yazılmıştır.
Elektronik devrenin hayata geçirilmesi ve testler Seçkin Albamya - Oğuzhan Taşdemir tarafından yapılmıştır.

Proje tanıtım videosu : https://youtu.be/n3OITI3CZNc

Donanım konusunda bilgilendirme:
*Sistem potansiyometrelerdeki gerilimi ölçerek konumu öğrenmek ve trim değerlerini ölçülen değerlere ekleyerek çıkış vermek üzerine kurulmuştur.
*Joystick mekaniğinde yer alan potansiyometreler şemaya uygun şekilde bağlanmalıdır.
*Butonlar için pulldown dirençleri önemlidir. Sistemin kararlı çalışması için pulldown dirençleri kullanılmalıdır. 1K - 10K aralığında direnç kullanılabilir. (Şemada 10K olarak belirtilmiştir.)
*AUX1 ve AUX 2 için 1K - 10K arasında potansiyometre kullanarak analog çıkış elde edebilirsiniz. Yerlerine buton veya iki konumlu switch'i AUX3 veya AUX4'teki gibi bağlayarak da kullanabilirsiniz.
*AUX3 ve AUX4 için şemada buton bağlantısı yapılmıştır. Buton yerine iki konumlu switch de kullanılabilir.
*10. pinden çıkış sinyali PPM olarak verilmektedir. Kumandanıza uygun soket kullanarak sinyali bağlayabilirsiniz. Birbirinden bağımsız iki elektronik devrenin çalışması için Joystick'in
GND pini ile kumandanın GND pinini birleştirmeniz gerektiğini unutmayın.(Trainer soketinde GND pini bulunmaktadır.)
12. pine buzzer bağlayarak trim ve otokalibrasyon mekanizmasının sesli bildirimlerini dinleyebilirsiniz.

Ek bilgiler:
*Aileron ve elevator dışındaki tüm girişler opsiyoneldir. İstenildiğinde konfigürasyon bölümüne // eklenerek kapatılabilir.

Otokalibrasyon:
*Aieleron trim butonlarının her ikisine de basılarak elektrik verildiğinde kesintisiz buzzer sesi duyulur. 10 saniye boyunca duyulacak olan bu ses kalibrasyonun başladığını belirtir. Aileron,
elevator , throttle (tanımlı ise) , rudder (tanımlı ise) max ve minimum noktalarına getirilir. Yazılım max ve min noktalarını hafızaya kaydeder. 10 saniye sonunda buzzer sesi kesilir ve 2 saniye
boyunca tekrar aktif hale gelir. Bu aşamada eksenler (throttle hariç) merkez noktada bırakılır ve yazılıma eksenlerin merkez noktası tanımlanmış olur. 2 saniye dolduğunda kalibrasyon 
tamamlanır ve yazılım çalışmaya başlar.

Teşekkürler:
*Oğuzhan Taşdemir'e ve INMUK üyelerine çalışmada verdiği destekten dolayı teşekkürler.

MIT lisansı ile paylaşılan Christopher Schirner (schinken)'in PPMEncoder kütüphanesi kullanılarak PPM çıkışı sağlanmıştır.*/

#include "PPMEncoder.h"
#include <EEPROM.h>

/*Konfigürasyonunuzu aşağıdaki bölümde bulunan // işaretlerini silerek veya ekleyerek ayaralayınız.
Eğer özelliğin başında // işareti varsa o özellik pasif, yoksa aktif olarak seçilmiştir. Yazılımın stabil çalışması için lütfen kullanmadığınız özelliği kapatınız.*/

#define Throttle
#define Rudder
//#define AUX1
//#define AUX2
//#define AUX3
//#define AUX4

//Arduino bağlantı pini ayarlamalarını aşağıdan yapabilirsiniz. Aşağıda belirtilen nota uygun şekilde pin dağılımını değiştirmek isterseniz değiştirebilirsiniz.
// Potansiyometre girişleri Analog (A ile başlayan) pinlere bağlanmak zorundadır.


#define ailpot A0 //Aileron potansiyometre pini
#define ailbuta 3 // Aileron buton pini
#define ailbutb 2 // Aileron buton pini

#define elepot A1 //Elevator potansiyometre pini
#define elebuta 5 // Elevator buton pini
#define elebutb 4 // Elevator buton pini
// Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
#ifdef Throttle 
  #define thrpot A2 //Throttle potansiyometre pini
  #define thrbuta 7 // Throttle buton pini
  #define thrbutb 6 // Throttle buton pini
#endif
// Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
#ifdef Rudder
  #define rudpot A3 //Rudder potansiyometre pini
  #define rudbuta 8 //Rudder potansiyometre pini
  #define rudbutb 9 //Rudder potansiyometre pini
#endif
// Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
#ifdef AUX1
  #define sw1 A4 // AUX1 potansiyometre pini
#endif

#ifdef AUX2
  #define sw2 A5 // AUX2 potansiyometre pini
#endif

#ifdef AUX3
  #define sw3 11 // AUX3 switch pini
#endif

#ifdef AUX4
  #define sw4 13 // AUX4 switch pini
#endif

#define buzzer 12 // Buzzer pini
#define ppmout 10 // Sinyal çıkış pini


//trim butonlarının çıkarttığı sesin süresini aşağıdaki parametreler ile ayarlayabilirsiniz.
#define longbuzzertime 200
#define shortbuzzertime 60

//Trim butonuna basılı tutulduğunda verilecek iki trim arası süre
#define trimtime 200

//Trim butonlarına basıldığı zaman verilecek trim miktarını her eksen için aşağıdaki değeri değiştirerek ayarlayabilirsiniz.
#define ailtrimamount 5 //Aileron
#define eletrimamount 5 //Elevator
#ifdef Throttle // Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
  #define thrtrimamount 5 //Throttle
#endif
#ifdef Rudder // Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
  #define rudtrimamount 5 // Rudder
#endif

//verilebilecek trim sayısı (verilecek trim miktarı yukarıdan ayarlanıyor.)
#define ailtrimrange 15 // Aileron
#define eletrimrange 15 // Rudder
#ifdef Throttle // Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
  #define thrtrimrange 15 //Throttle
#endif
#ifdef Rudder // Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
  #define rudtrimrange 15 // Rudder
#endif

//Reverse ayarları
//Normal için 1 , Reverse için -1 yapın.
#define ailreverse 1 // Aileron
#define elereverse -1 // Rudder
#ifdef Throttle // Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
  #define thrreverse -1 //Throttle
#endif
#ifdef Rudder// Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
  #define rudreverse 1 // Rudder
#endif

//Aşağıdaki sayıyı değiştirerek eksenlerin çıkış aralığını değiştirebilirsiniz.(min ve max aralığı eşit şekilde genişler veya daralır. 1.0 için PPM çıkışı 1000-2000 arası olarak ayarlanmıştır.)
#define ailrange 1.0 // Aileron
#define elerange 1.0 // Rudder
#ifdef Throttle // Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
  #define thrrange 1.0 //Throttle
#endif
#ifdef Rudder // Tanımlama yapılmamışsa ayarlamanıza gerek yoktur.
  #define rudrange 1.0 // Rudder
#endif

//Tanımlama bölümü sonu. Aşağıda kodlar bulunmaktadır.
//////////////////////////////////////////////////////////////////////////////

int CH1 ,CH2 ,CH3 ,CH4 ,CH5 , CH6 , CH7, CH8;
int trimrange = 5 , buzzersoundtime = shortbuzzertime;
int ailtrim = 0 , eletrim = 0 , thrtrim = 0 , rudtrim = 0;
long int buzzertimer , holdingtimer = 0;

void trimread()
{
/////aileron

  if( digitalRead(ailbuta) == HIGH && millis() - holdingtimer > trimtime )
    {
      if( ailtrim < ailtrimrange * ailtrimamount )
        ailtrim = ailtrim + ailtrimamount;
      buzzeronail();
      holdingtimer = millis();
    }

  if( digitalRead(ailbutb) == HIGH  && millis() - holdingtimer > trimtime )
    {
      if( ailtrim > -ailtrimrange * ailtrimamount )
        ailtrim = ailtrim - ailtrimamount;
      buzzeronail();
      holdingtimer = millis();
    }
/////elevator
    if( digitalRead(elebuta) == HIGH && millis() - holdingtimer > trimtime )
    {
      if( eletrim < eletrimrange * eletrimamount )
        eletrim = eletrim + eletrimamount;
      buzzeronele();
      holdingtimer = millis();
    }

  if( digitalRead(elebutb) == HIGH  && millis() - holdingtimer > trimtime )
    {
      if( eletrim > -eletrimrange * eletrimamount )
        eletrim = eletrim - eletrimamount;
      buzzeronele();
      holdingtimer = millis();
    }
#ifdef Throttle
  /////throttle
    if( digitalRead(thrbuta) == HIGH && millis() - holdingtimer > trimtime )
    {
      if( thrtrim < thrtrimrange * thrtrimamount )
        thrtrim = thrtrim + thrtrimamount;
      buzzeronthr();
      holdingtimer = millis();
    }

  if( digitalRead(thrbutb) == HIGH  && millis() - holdingtimer > trimtime )
    {
      if( thrtrim > -thrtrimrange * thrtrimamount )
        thrtrim = thrtrim - thrtrimamount;
      buzzeronthr();
      holdingtimer = millis();
    }
#endif

#ifdef Rudder
  /////rudder
    if( digitalRead(rudbuta) == HIGH && millis() - holdingtimer > trimtime )
    {
      if( rudtrim < rudtrimrange * rudtrimamount )
        rudtrim = rudtrim + rudtrimamount;
      buzzeronrud();
      holdingtimer = millis();
    }

  if( digitalRead(rudbutb) == HIGH  && millis() - holdingtimer > trimtime )
    {
      if( rudtrim > -rudtrimrange * rudtrimamount )
        rudtrim = rudtrim - rudtrimamount;
      buzzeronrud();
      holdingtimer = millis();
    }
#endif

} 

void buzzeronail()
{
  
  digitalWrite(buzzer,HIGH);
  buzzertimer = millis();
  
  if( ailtrim == 0 || ailtrim <= -ailtrimrange * ailtrimamount || ailtrim >= ailtrimrange * ailtrimamount )
    buzzersoundtime = longbuzzertime;
  else if (ailtrim != 0)
    buzzersoundtime = shortbuzzertime;
}

void buzzeronele()
{
  
  digitalWrite(buzzer,HIGH);
  buzzertimer = millis();
  
  if( eletrim == 0 || eletrim <= -eletrimrange * eletrimamount || eletrim >= eletrimrange * eletrimamount )
    buzzersoundtime = longbuzzertime;
  else if (eletrim != 0)
    buzzersoundtime = shortbuzzertime;
}

#ifdef Throttle
void buzzeronthr()
{
  
  digitalWrite(buzzer,HIGH);
  buzzertimer = millis();
  
  if( thrtrim == 0 || thrtrim <= -thrtrimrange * thrtrimamount || thrtrim >= thrtrimrange * thrtrimamount )
    buzzersoundtime = longbuzzertime;
  else if (thrtrim != 0)
    buzzersoundtime = shortbuzzertime;
}
#endif

#ifdef Rudder
void buzzeronrud()
{
  
  digitalWrite(buzzer,HIGH);
  buzzertimer = millis();
  
  if( rudtrim == 0 || rudtrim <= -rudtrimrange * rudtrimamount || rudtrim >= rudtrimrange * rudtrimamount )
    buzzersoundtime = longbuzzertime;
  else if (rudtrim != 0)
    buzzersoundtime = shortbuzzertime;
}
#endif

void buzzeroff()
{
  
  if(millis() - buzzertimer > buzzersoundtime )
    digitalWrite(buzzer,LOW);
    
}

void joystickposition()
{
  if( analogRead(ailpot) < EEPROM.read(2)*4 )
    CH1 = ailtrim + map(analogRead(ailpot) , EEPROM.read(0)*4 , EEPROM.read(2)*4 ,  (1500 - ailrange * ailreverse * 500) , 1500 );
  else if( analogRead(ailpot) >= EEPROM.read(2)*4 )
    CH1 = ailtrim + map( analogRead(ailpot) , EEPROM.read(2)*4 , EEPROM.read(1)*4 , 1500 , (1500 + ailrange * ailreverse * 500) );
    
  if( analogRead(elepot) < EEPROM.read(5)*4 )
    CH2 = eletrim + map(analogRead(elepot) , EEPROM.read(3)*4 , EEPROM.read(5)*4 , (1500 - elerange * elereverse * 500) , 1500 );
  else if( analogRead(elepot) >= EEPROM.read(5)*4 )
    CH2 = eletrim + map( analogRead(elepot) , EEPROM.read(5)*4 , EEPROM.read(4)*4 , 1500 , (1500 + elerange * elereverse * 500) );
    
  #ifdef Throttle
  CH3 = thrtrim + map(analogRead(thrpot) , EEPROM.read(6)*4 , EEPROM.read(7)*4 , thrrange * (1500 - thrreverse*500) , thrrange * (1500 + thrreverse*500) );
  #endif
  
  #ifdef Rudder
  if( analogRead(rudpot) < EEPROM.read(11)*4 )
    CH4 = rudtrim + map(analogRead(rudpot) , EEPROM.read(9)*4 , EEPROM.read(11)*4 , (1500 - rudrange * rudreverse * 500) , 1500 );
  else if( analogRead(rudpot) >= EEPROM.read(11)*4 )
    CH4 = rudtrim + map( analogRead(rudpot) , EEPROM.read(11)*4 , EEPROM.read(10)*4 , 1500 , (1500 + rudrange * rudreverse * 500) );
  #endif
  
  #ifdef AUX1
  CH5 = map(analogRead(sw1) , 0 , 1023 , 1000 , 2000);
  #endif
   #ifdef AUX2
  CH6 = map(analogRead(sw2) , 0 , 1023 , 1000 , 2000);
  #endif
   #ifdef AUX3
  CH7 = map(digitalRead(sw3) , 0 , 1 , 1000 , 2000);
  #endif
   #ifdef AUX4
  CH8 = map(digitalRead(sw4) , 0 , 1 , 1000 , 2000);
  #endif
}

void ppm()
{
  
  ppmEncoder.setChannel( 0 , CH1 );
  ppmEncoder.setChannel( 1 , CH2 );
  ppmEncoder.setChannel( 2 , CH3 );
  ppmEncoder.setChannel( 3 , CH4 );  
  ppmEncoder.setChannel( 4 , CH5 );
  ppmEncoder.setChannel( 5 , CH6 );
  ppmEncoder.setChannel( 6 , CH7 );
  ppmEncoder.setChannel( 7 , CH8 );
  
 }

 void calibration()
 {
  int calibrationtime;
  int ailcal[3] , elecal[3];
  #ifdef Throttle
  int thrcal[3];
  #endif
  #ifdef Rudder
  int rudcal[3];
  #endif
  
  calibrationtime = millis();

  digitalWrite(buzzer,HIGH);

  ailcal[0] = analogRead(ailpot);
  ailcal[1] = analogRead(ailpot);

  elecal[0] = analogRead(elepot);
  elecal[1] = analogRead(elepot);

  #ifdef Throttle
  thrcal[0] = analogRead(thrpot);
  thrcal[1] = analogRead(thrpot);
  #endif

  #ifdef Rudder
  rudcal[0] = analogRead(rudpot);
  rudcal[1] = analogRead(rudpot);
  #endif
  
  while(millis() - calibrationtime < 10000)
    {
      //aileron calibration
      if(analogRead(ailpot) < ailcal[0] )//min
        ailcal[0] = analogRead(ailpot); 
      if(analogRead(ailpot) > ailcal[1] )//max
        ailcal[1] = analogRead(ailpot);
        
        //eleron calibration
      if(analogRead(elepot) < elecal[0] )//min
        elecal[0] = analogRead(elepot); 
      if(analogRead(elepot) > elecal[1] )//max
        elecal[1] = analogRead(elepot);

      #ifdef Throttle
        //throttle calibration
      if(analogRead(thrpot) < thrcal[0] )//min
        thrcal[0] = analogRead(thrpot); 
      if(analogRead(thrpot) > thrcal[1] )//max
        thrcal[1] = analogRead(thrpot);
      #endif
      
      #ifdef Rudder  
        //rudder calibration
      if(analogRead(rudpot) < rudcal[0] )//min
        rudcal[0] = analogRead(rudpot); 
      if(analogRead(rudpot) > rudcal[1] )//max
        rudcal[1] = analogRead(rudpot);
      #endif   
    }

    digitalWrite(buzzer,LOW);
    delay(500);
    digitalWrite(buzzer,HIGH);
    
    calibrationtime = millis();
    
    while(millis() - calibrationtime < 1000)
    {
      //center point
      ailcal[2] = analogRead(ailpot);
      elecal[2] = analogRead(elepot);
      //thr ortası yok
      #ifdef Rudder  
      rudcal[2] = analogRead(rudpot);
      #endif
    }

      EEPROM.write(0,ailcal[0]/4);
      EEPROM.write(1,ailcal[1]/4);
      EEPROM.write(2,ailcal[2]/4);
      
      EEPROM.write(3,elecal[0]/4);
      EEPROM.write(4,elecal[1]/4);
      EEPROM.write(5,elecal[2]/4);
      
      #ifdef Throttle
      EEPROM.write(6,thrcal[0]/4);
      EEPROM.write(7,thrcal[1]/4);
      #endif
      #ifdef Rudder
      EEPROM.write(9,rudcal[0]/4);
      EEPROM.write(10,rudcal[1]/4);
      EEPROM.write(11,rudcal[2]/4);
      #endif
 }

void setup() {
  
  pinMode(ailpot,INPUT);
  pinMode(ailbuta,INPUT);
  pinMode(ailbutb,INPUT);
  
  pinMode(elepot,INPUT);
  pinMode(elebuta,INPUT);
  pinMode(elebutb,INPUT);
  
#ifdef thrpot
  pinMode(thrpot,INPUT);
  pinMode(thrbuta,INPUT);
  pinMode(thrbutb,INPUT);
#endif

#ifdef rudpot
  pinMode(rudpot,INPUT);
  pinMode(rudbuta,INPUT);
  pinMode(rudbutb,INPUT);
#endif  

#ifdef AUX1
  pinMode(sw1,INPUT);
#endif
  
#ifdef AUX2
  pinMode(sw2,INPUT);
#endif  

#ifdef AUX3
  pinMode(sw3,INPUT);
#endif  

#ifdef AUX4
  pinMode(sw4,INPUT);
#endif  

  pinMode(buzzer,OUTPUT);
  
  ppmEncoder.begin(ppmout);

  if( digitalRead(ailbuta) == HIGH && digitalRead(ailbutb) == HIGH)
    calibration();

}

void loop() 
{
  
  trimread();
  buzzeroff();
  joystickposition();
  ppm();
  
}
