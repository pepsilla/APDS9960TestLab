import controlP5.*;
import processing.serial.*;


ControlP5 cp5;
Textarea myTextarea;
DropdownList com;
DropdownList baud;
DropdownList again;
DropdownList pgain;
DropdownList pplen;
DropdownList gdims;
DropdownList gplen;
DropdownList ledboost;
DropdownList gldrive;
DropdownList gexpers;
DropdownList gexmsk;
DropdownList gfifoth;
Toggle chip;
Toggle pon;
Toggle aon;
Toggle gon;
Toggle gmode;
Numberbox ppulse;
Numberbox delay;
Numberbox gexth;
Numberbox gpenth;
Numberbox gpulse;
Numberbox gwtime;

int APDS9960Buffer[] =
{
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
String otherValues;
Serial miPuerto;
int Baud = 460800;

String _textInfo = new String();
Boolean _textMode = false;
int _level;
String[] arduinos;

void setup(){
  size(800, 650);
  ControlP5 cp5 = new ControlP5(this);
  surface.setTitle("APDS9660_TestLab@pepsilla");

  //miPuerto = new Serial(this, "COM10",Baud);//Serial.list()[Puerto], Baud);  // Abre el puerto que se esté utilizando
  //miPuerto.bufferUntil('\n');  // No genera un serialEvent() a menos que se obtenga un caracter de nueva línea ('\n')
  
  myTextarea = cp5.addTextarea("txt")
                  .setPosition(100,100)
                  .setSize(600,400)
                  .setFont(createFont("arial",12))
                  .setLineHeight(14)
                  .setColor(color(128))
                  .setColorBackground(color(255,100))
                  .setColorForeground(color(255,100));
                  ;


  // COM dropdown
  com = cp5.addDropdownList("com")
    .setPosition(210, 20)
    .setSize(50,200);
    
   //Baud dropdown
   baud = cp5.addDropdownList("baud")
    .setPosition(270, 20)
    .setSize(60,200);
  
   //AGAIN dropdown
   again = cp5.addDropdownList("AGAIN")
    .setPosition(340, 20)
    .setSize(50,200);
  
  //PGAIN dropdown
   pgain = cp5.addDropdownList("PGAIN")
    .setPosition(400, 20)
    .setSize(50,200);
  
  //PPLEN dropdown
   pplen = cp5.addDropdownList("PPLEN")
    .setPosition(460, 20)
    .setSize(50,200);
    
  //gdims dropdown
   gdims = cp5.addDropdownList("GDIMS")
    .setPosition(520, 20)
    .setSize(50,200);
  //gplen dropdown
   gplen = cp5.addDropdownList("GPLEN")
    .setPosition(580, 20)
    .setSize(50,200);
  
  //ledboost dropdown
   ledboost = cp5.addDropdownList("LEDBOOST")
    .setPosition(340, 510)
    .setSize(50,200);
  
  //gldrive dropdown
   gldrive = cp5.addDropdownList("GLDRIVE")
    .setPosition(400, 510)
    .setSize(50,200);
  //gexpers dropdown
   gexpers = cp5.addDropdownList("GEXPERS")
    .setPosition(460, 510)
    .setSize(50,200);
  //gexmks dropdown
   gexmsk = cp5.addDropdownList("GEXMSK")
    .setPosition(520, 510)
    .setSize(50,200);
  //gfifoth dropdown
   gfifoth = cp5.addDropdownList("GFIFOTH")
    .setPosition(580, 510)
    .setSize(50,200);
    
  //ON/OFF BUTTON
  chip = cp5.addToggle("CHIP")
     .setPosition(40,10)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
   
   //PON/POFF BUTTON
  pon = cp5.addToggle("PROXIMITY")
     .setPosition(40,60)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
     //AON/POFF BUTTON
  aon = cp5.addToggle("ALS_RGB")
     .setPosition(40,110)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
     
  gon = cp5.addToggle("GESTURE")
     .setPosition(40,160)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
  gmode = cp5.addToggle("GMODE")
     .setPosition(40,210)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
  
  //PPLEN values
  delay = cp5.addNumberbox("DELAY")
             .setPosition(40,260)
             .setSize(40,14)
             .setScrollSensitivity(1)
             .setRange(0,1000)
             .setValue(100)
             ;
  //GEXTH values
  gexth = cp5.addNumberbox("GEXTH")
             .setPosition(40,300)
             .setSize(40,14)
             .setScrollSensitivity(1)
             .setRange(0,255)
             .setValue(0)
             ;           
  //GPENTH values
  gpenth = cp5.addNumberbox("GPENTH")
             .setPosition(40,340)
             .setSize(40,14)
             .setScrollSensitivity(1)
             .setRange(0,255)
             .setValue(100)
             ;
//PPLEN values
  ppulse = cp5.addNumberbox("PPULSE")
             .setPosition(40,380)
             .setSize(40,14)
             .setScrollSensitivity(1)
             .setRange(0,63)
             .setValue(0)
             ;             
  //gpulse values
   gpulse = cp5.addNumberbox("GPULSE")
    .setPosition(40,520)
             .setSize(40,410)
             .setScrollSensitivity(1)
             .setRange(0,64)
             .setValue(100)
             ;
//gwtime values
   gwtime = cp5.addNumberbox("GWTIME")
    .setPosition(40,450)
             .setSize(40,14)
             .setScrollSensitivity(1)
             .setRange(0,7)
             .setValue(100)
             ;             
             
  //Add com ports to dropdown listt
  arduinos = Serial.list();
  for (int i=0; i<arduinos.length; i++)
  {
    com.addItem(arduinos[i], i);
  }
  
  //Add Baud rates to dropdown
  baud.addItem("300",0);
  baud.addItem("600",1);
  baud.addItem("1200",2);
  baud.addItem("1800",3);
  baud.addItem("2400",4);
  baud.addItem("3600",5);
  baud.addItem("4800",6);
  baud.addItem("7200",7);
  baud.addItem("9600",8);
  baud.addItem("14400",9);
  baud.addItem("19200",10);
  baud.addItem("28800",11);
  baud.addItem("38400",12);
  baud.addItem("57600",13);
  baud.addItem("74880",14);
  baud.addItem("115200",15);
  baud.addItem("230400",16);
  baud.addItem("250000",17);
  baud.addItem("460800",18);
  baud.addItem("921600",19);
  baud.setValue(18);
  baud.setLabel("460800");
  baud.close();
  
  again.addItem("1x",0);
  again.addItem("4x",1);
  again.addItem("16x",2);
  again.addItem("64x",3);
  again.close();
  
  pgain.addItem("1x",0);
  pgain.addItem("2x",1);
  pgain.addItem("4x",2);
  pgain.addItem("8x",3);
  pgain.close();
  
  pplen.addItem("4us",0);
  pplen.addItem("8us",1);
  pplen.addItem("16us",2);
  pplen.addItem("32us",3);
  pplen.close();
  
  gdims.addItem("U/D & L/R",0);
  gdims.addItem("U/D Only",1);
  gdims.addItem("L/R Only",2);
  gdims.addItem("U/D & L/R",3);
  gdims.close();
  
  gplen.addItem("4 us",0);
  gplen.addItem("8 us",1);
  gplen.addItem("16 us",2);
  gplen.addItem("32 us",3);
  gplen.close();
  
  ledboost.addItem("100%",0);
  ledboost.addItem("150%",1);
  ledboost.addItem("200%",2);
  ledboost.addItem("300%",3);
  ledboost.close();
  
  gldrive.addItem("100 mA",0);
  gldrive.addItem("50 mA",1);
  gldrive.addItem("25 mA",2);
  gldrive.addItem("12.5 mA",3);
  gldrive.close();
  
  gexpers.addItem("1st",0);
  gexpers.addItem("2nd",1);
  gexpers.addItem("4th",2);
  gexpers.addItem("7th",3);
  gexpers.close();
  
  gfifoth.addItem("a1d",0);
  gfifoth.addItem("a4d",1);
  gfifoth.addItem("a8d",2);
  gfifoth.addItem("a16d",3);
  gfifoth.close();
  
  gexmsk.addItem("ALL",0);
  gexmsk.addItem("R excluded",1);
  gexmsk.addItem("L excluded",2);
  gexmsk.addItem("D excluded",3);
  gexmsk.addItem("U excluded",4);
  gexmsk.addItem("---",5);
  gexmsk.addItem("L&D excluded",6);
  gexmsk.addItem("ALL excluded",7);
  gexmsk.close();
  
  hideChipControls();
}

void draw(){
  background(0);
  //frame.setTitle("SerialWHEATER:"+otherValues);

}


void controlEvent(ControlEvent theEvent) {
  int val = int(theEvent.getValue());

  if (theEvent.getName() == "com"){
    println(int(baud.getValue()));
    if(miPuerto!=null)
    {
      miPuerto.stop();
      hideChipControls();
      miPuerto = new Serial(this, arduinos[val],Baud);//Serial.list()[Puerto], Baud);  // Abre el puerto que se esté utilizando
      miPuerto.bufferUntil('\n');  // No genera un serialEvent() a menos que se obtenga un caracter de nueva línea ('\n')
      showChipControls();
    }
    else
    {
      miPuerto = new Serial(this, arduinos[val],Baud);//Serial.list()[Puerto], Baud);  // Abre el puerto que se esté utilizando
      miPuerto.bufferUntil('\n');  // No genera un serialEvent() a menos que se obtenga un caracter de nueva línea ('\n')
      showChipControls();
    }
  }else if (theEvent.getName() == "baud"){
    println(val);
  }else if (theEvent.getName() == "AGAIN"){
    if(miPuerto!=null)
    {
      miPuerto.write("AGAIN ");
      miPuerto.write(str(int(val)));
      miPuerto.write("\r\n");
    }
  }else if (theEvent.getName() == "PGAIN"){
    if(miPuerto!=null)
    {
      miPuerto.write("PGAIN ");
      miPuerto.write(str(int(val)));
      miPuerto.write("\r\n");
    }
  }
  else if (theEvent.getName() == "PROXIMITY"){
    if(miPuerto!=null)
    {
      if(int(val)==1)miPuerto.write("PON\r\n");
      else miPuerto.write("POFF\r\n");
    }
  }else if (theEvent.getName() == "ALS_RGB"){
    if(miPuerto!=null)
    {
      if(int(val)==1)miPuerto.write("AON\r\n");
      else miPuerto.write("AOFF\r\n");
    }
  }else if (theEvent.getName() == "CHIP"){
    if(miPuerto!=null)
    {
      if(int(val)==1)miPuerto.write("ON\r\n");
      else miPuerto.write("OFF\r\n");
    }
  }else if (theEvent.getName() == "PPULSE"){
    if(miPuerto!=null)
    {
      miPuerto.write("PPULSE ");
      miPuerto.write(str(int(val)));
      miPuerto.write("\r\n");
    }
  }else if (theEvent.getName() == "PPLEN"){
    if(miPuerto!=null)
    {
      miPuerto.write("PPLEN ");
      miPuerto.write(str(int(val)));
      miPuerto.write("\r\n");
    }
  }else if (theEvent.getName() == "DELAY"){
    if(miPuerto!=null)
    {
      miPuerto.write("DELAY ");
      miPuerto.write(str(int(val)));
      miPuerto.write("\r\n");
    }
  }else if (theEvent.getName() == "GMODE"){
    if(miPuerto!=null)
    {
      miPuerto.write("GMODE ");
      miPuerto.write(str(int(val)));
      miPuerto.write("\r\n");
    }
  }else if (theEvent.getName() == "GESTURE"){
    if(miPuerto!=null)
    {
      if(int(val)==1)miPuerto.write("GON\r\n ");
      else miPuerto.write("GOFF\r\n");
    }
  }else if (theEvent.getName() == "GDIMS"){
    if(miPuerto!=null)
    {
      if(int(val)<4)
      {
        miPuerto.write("GDIMS ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }else if (theEvent.getName() == "GPLEN"){
    if(miPuerto!=null)
    {
      if(int(val)<4)
      {
        miPuerto.write("GPLEN ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }else if (theEvent.getName() == "GPULSE"){
    if(miPuerto!=null)
    {
      if(int(val)<64)
      {
        miPuerto.write("GPULSE ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }else if (theEvent.getName() == "GLDRIVE"){
    if(miPuerto!=null)
    {
      if(int(val)<4)
      {
        miPuerto.write("GLDRIVE ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }else if (theEvent.getName() == "GEXPERS"){
    if(miPuerto!=null)
    {
      if(int(val)<4)
      {
        miPuerto.write("GEXPERS ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }else if (theEvent.getName() == "GEXMSK"){
    if(miPuerto!=null)
    {
      if(int(val)<16)
      {
        miPuerto.write("GEXMSK ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }else if (theEvent.getName() == "GFIFOTH"){
    if(miPuerto!=null)
    {
      if(int(val)<4)
      {
        miPuerto.write("GFIFOTH ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }else if (theEvent.getName() == "GEXTH"){
    if(miPuerto!=null)
    {
      if(int(val)<256)
      {
        miPuerto.write("GEXTH ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }else if (theEvent.getName() == "GPENTH"){
    if(miPuerto!=null)
    {
      if(int(val)<256)
      {
        miPuerto.write("GPENTH ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }else if (theEvent.getName() == "LEDBOOST"){
    if(miPuerto!=null)
    {
      if(int(val)<4)
      {
        miPuerto.write("LEDBOOST ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }else if (theEvent.getName() == "GWTIME"){
    if(miPuerto!=null)
    {
      if(int(val)<8)
      {
        miPuerto.write("GWTIME ");
        miPuerto.write(str(int(val)));
        miPuerto.write("\r\n");
      }
     }
  }
  
  
  if (theEvent.isGroup()) {
    // check if the Event was triggered from a ControlGroup
    println("event from group : "+theEvent.getGroup().getValue()+" from "+theEvent.getGroup());
  } 
  else if (theEvent.isController()) {
    println("event from controller : "+theEvent.getController().getValue()+" from "+theEvent.getController());
  }
}

void serialEvent (Serial miPuerto) { // Lee los datos capturados por el puerto del Arduino

     String StringEntrante = miPuerto.readStringUntil('\n');  // Obtiene la cadena ASCII
     if (StringEntrante != null) {
         if(StringEntrante.indexOf("APDS9960@pepsilla_TestLab")==0)
         {
           print("");
           _textInfo = new String();
           _textMode = true;
           _level=0;
           _textInfo += StringEntrante;
           return;
         }
         if(_textMode)
         {
           if(StringEntrante.indexOf("{")>=0)_level++;
           if(StringEntrante.indexOf("}")>=0)_level--;
           if(_level==0)
           {
             _textMode = false;
             _level = 0;
             _textInfo += StringEntrante;
             myTextarea.setText (_textInfo);
           }
           else  _textInfo += StringEntrante;
         }
         else
         {

           String StringValores[] = split(StringEntrante,":");  // divide valores
           print(StringValores);
           int parValues = StringValores.length;
           int fila = 0;
           if(parValues==2)
           {
             fila = int(StringValores[0]);
             if(fila<16)
             {
               String columnas[] = split(parseString(StringValores[1]),",");
               int ncol = columnas.length;
               if(ncol<16){
                 for(int contador=0;contador<ncol;contador++)
                 {
                   APDS9960Buffer[fila*16+contador]=int(columnas[contador]);
                 }
               }
             }
           }
         }
     }
  }
  String parseString(String value)
  {
    String returnString = new String();
    int counter;
    int init = 0;
    int end = value.length();
    if(value.charAt(0)=='"')init++;
    if(value.charAt(end-1)=='"')end--;
    for (counter=init;counter<end;counter++)
    {
      returnString += value.charAt(counter);
    }
     return returnString;
}

void hideChipControls()
{
  myTextarea.hide();
  again.hide();
  pgain.hide();
  chip.hide();
  pon.hide();
  aon.hide();
  ppulse.hide();
  pplen.hide();
  delay.hide();
  gmode.hide();
  gon.hide();
  gdims.hide();
  gplen.hide();
  gpulse.hide();
  ledboost.hide();
  gwtime.hide();
  gldrive.hide();
  gexpers.hide();
  gexmsk.hide();
  gfifoth.hide();
  gexth.hide();
  gpenth.hide();
}

void showChipControls()
{
  myTextarea.show();
  again.show();
  pgain.show();
  chip.show();
  pon.show();
  aon.show();
  ppulse.show();
  pplen.show();
  delay.show();
  gmode.show();
  gon.show();
  gdims.show();
  gplen.show();
  gpulse.show();
  ledboost.show();
  gwtime.show();
  gldrive.show();
  gexpers.show();
  gexmsk.show();
  gfifoth.show();
  gexth.show();
  gpenth.show();
}