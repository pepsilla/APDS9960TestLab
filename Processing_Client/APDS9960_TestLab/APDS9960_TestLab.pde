import controlP5.*;
import processing.serial.*;


ControlP5 cp5;
Textarea myTextarea;
DropdownList com;
DropdownList baud;
DropdownList again;
DropdownList pgain;
DropdownList pplen;
Toggle chip;
Toggle pon;
Toggle aon;
Toggle gon;
Toggle gmode;
Numberbox ppulse;
Numberbox delay;

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
  size(800, 600);
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
    
  //ON/OFF BUTTON
  chip = cp5.addToggle("CHIP")
     .setPosition(40,150)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
   
   //PON/POFF BUTTON
  pon = cp5.addToggle("PROXIMITY")
     .setPosition(40,200)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
     //AON/POFF BUTTON
  aon = cp5.addToggle("ALS_RGB")
     .setPosition(40,250)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
     
  gon = cp5.addToggle("GESTURE")
     .setPosition(40,300)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
  gmode = cp5.addToggle("GMODE")
     .setPosition(40,350)
     .setSize(50,20)
     .setValue(false)
     .setMode(ControlP5.SWITCH)
     ;
  //PPLEN values
  ppulse = cp5.addNumberbox("PPULSE")
             .setPosition(40,525)
             .setSize(40,14)
             .setScrollSensitivity(1)
             .setRange(0,63)
             .setValue(0)
             ;
  //PPLEN values
  delay = cp5.addNumberbox("DELAY")
             .setPosition(40,400)
             .setSize(40,14)
             .setScrollSensitivity(1)
             .setRange(0,1000)
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
}