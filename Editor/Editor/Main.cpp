#include<Siv3D.hpp>
#include<Windows.h>

void update();
void draw();
void drawEdit(int sX,int sY);

int lcm(int, int);
void import(String path);
void saveFile(String path);

void generateIsClicked();
void nextMeasure();
void prevMeasure();
void jumpMeasure(int measure);


class NormalNote {
public:
  NormalNote() {};
  NormalNote(int split, int y, int x, int width, bool isEx) {
    this->split = split; this->y = y; this->x = x; this->width = width; this->isEx = isEx;
  };

  int split;
  int y,x;
  int width;
  bool isEx;
};

class SlideNote {
public:
  SlideNote() {};
  SlideNote(int split,int y,int startX,int endX,int startWidth,int endWidth,int longSplit,int length,bool isTap) {
    this->split = split; this->y = y; this->startX = startX; this->endX = endX; this->startWidth = startWidth; this->endWidth = endWidth; this->longSplit = longSplit; this->length = length; this->isTap = isTap;
  };

  int split;
  int y,startX,endX;
  int startWidth, endWidth;
  int longSplit;
  int length;
  bool isTap;
};

class FlickNote {
public:
  FlickNote() {};
  FlickNote(int split, int y, int x, int width) {
    this->split = split; this->y = y; this->x = x; this->width = width;
  };

  int split;
  int y, x;
  int width;
};

class PedalNote {
public:
  PedalNote() {};
  PedalNote(int split, int y, int longSplit, int length) { this->split = split; this->y = y; this->longSplit = longSplit; this->length = length; };
  
  int split;
  int y;
  int longSplit;
  int length;

};

class Option {
public:
  Option() {};
  Option(int split, int y, bool isBpm, bool isBeat, bool isStop, double bpm, int beatSplit, int beat, int stopSplit, int stopLength) {
    this->split = split; this->y = y; this->isBpm = isBpm; this->isBeat = isBeat; this->isStop = isStop; this->bpm = bpm; this->beatSplit = beatSplit; this->beat = beat; this->stopSplit = stopSplit; this->stopLength = stopLength;
  };
  int split;
  int y;
  bool  isBpm = false, isBeat = false, isStop = false;
  double bpm;
  int beatSplit, beat;
  int stopSplit, stopLength;

};

class Measure {
public:
  std::vector<NormalNote> normalNotes;
  std::vector<SlideNote> slideNotes;
  std::vector<FlickNote> flickNotes;
  std::vector<PedalNote> pedalNotes;
  std::vector<Option> options;
};

class EditRects {
public:
  std::vector<std::vector<Rect>> click;
  std::vector<std::vector<Rect>> draw;
};

class FileOption {
public:
  FileOption() {};
  FileOption(String name, String text) { this->name = name; this->text = text; };
  String name;
  String text;
};

std::vector<FileOption> fileOptions;
std::array<String,11> optionNames= { L"TITLE", L"SUBTITLE", L"ARTIST", L"BPM", L"JACKET", L"MUSIC", L"DIFFICULTY", L"MOVIE", L"OFFSET",L"SELECTOFFSET",L"SELECTLONG"};

std::vector<Measure> measures;
int currentMeasure = 0;

EditRects edit32;
EditRects edit24;

std::vector<Rect> splitButtons;
std::vector<Texture> splitPics;
std::vector<Rect> moveButtons;
std::vector<Texture> movePics;
std::vector<Rect> noteTypeButtons;
std::vector<Texture> noteTypePics;
Rect importButton,saveButton;
Texture importPic,savePic;

int buttonWidth = 20;
int pedalWidth = 2 * buttonWidth;

int editStartX = 10;
int editStartY = 10;
int editWidth = 16 * buttonWidth + pedalWidth;
int editHeight = 96*6;
int editNoteHeight32 = editHeight / 32;
int editNoteHeight24 = editHeight / 24;

int laneWidths[] = { buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,buttonWidth,pedalWidth };
int laneStartXs[] = { editStartX,
editStartX + buttonWidth * 1,
editStartX + buttonWidth * 2,
editStartX + buttonWidth * 3,
editStartX + buttonWidth * 4,
editStartX + buttonWidth * 5,
editStartX + buttonWidth * 6,
editStartX + buttonWidth * 7,
editStartX + buttonWidth * 8,
editStartX + buttonWidth * 9,
editStartX + buttonWidth * 10,
editStartX + buttonWidth * 11,
editStartX + buttonWidth * 12,
editStartX + buttonWidth * 13,
editStartX + buttonWidth * 14,
editStartX + buttonWidth * 15,
editStartX + buttonWidth * 16
};

int split = 8;
int splitList[] = {4,8,16,32,3,6,12,24};
int splitStartX = editStartX + editWidth + 120;
int splitStartY = editStartY;

int moveStartX = (editWidth / 2) + editStartX - (40 / 2) * 3 - 10;
int moveStartY = editStartY + editHeight + 10;

int noteType = 0;
int noteTypeStartX = splitStartX;
int noteTypeStartY = splitStartY + 50 * 8 + 30;

bool jumpFlag = false, importFlag = false, saveFlag = false;
bool normalFlag = false, slideFlag = false, flickFlag = false, pedalFlag = false, optionFlag = false;

int currentX = 0, currentY = 0, currentSplit = 0;

Font f10,f20;

GUI jumpGUI,normalGUI,slideGUI,flickGUI,pedalGUI,optionGUI,importGUI,saveGUI;

void Main(){

#ifdef _DEBUG
  //デバッグ時のみコンソール出力を有効にする
  AllocConsole();
  FILE* out = 0;
  freopen_s(&out, "CON", "w", stdout);
  FILE* in = 0;
  freopen_s(&in, "CON", "r", stdin);
#endif // _DEBUG

  //----------
  Window::SetTitle(L"高専祭2018 譜面作成");
  Window::Resize(650, 650);
  Graphics::SetBackground(Color(40));
  System::SetExitEvent(WindowEvent::CloseButton);
  //----------

  for (int i = 32-1; i >= 0; --i) {
    std::vector<Rect> tmpC;
    std::vector<Rect> tmpD;
    std::vector<bool>tmpFlag;

    int cY = editStartY + editNoteHeight32 * i ;
    int dY = editStartY + editNoteHeight32 * i + editNoteHeight32 / 2;

    for (int j = 0; j < 17; ++j){
      tmpFlag.push_back(false);
      if (j == 16) {
        tmpC.push_back(Rect(editStartX + buttonWidth * j, cY, pedalWidth, editNoteHeight32));
        tmpD.push_back(Rect(editStartX + buttonWidth * j, dY, pedalWidth, editNoteHeight32 / 2));
      } else {
        tmpC.push_back(Rect(editStartX + buttonWidth * j, cY, buttonWidth, editNoteHeight32));
        tmpD.push_back(Rect(editStartX + buttonWidth * j, dY, buttonWidth, editNoteHeight32 / 2));
      }
    }

    edit32.click.push_back(tmpC);
    edit32.draw.push_back(tmpD);
  }

  for (int i = 24-1; i >= 0; --i) {
    std::vector<Rect> tmpC;
    std::vector<Rect> tmpD;
    std::vector<bool>tmpFlag;

    int cY = editStartY + editNoteHeight24 * i;
    int dY = editStartY + editNoteHeight24 * i + editNoteHeight24 / 2;

    for (int j = 0; j < 17; ++j) {
      tmpFlag.push_back(false);
      if (j == 16) {
        tmpC.push_back(Rect(editStartX + buttonWidth * j, cY, pedalWidth, editNoteHeight24));
        tmpD.push_back(Rect(editStartX + buttonWidth * j, dY, pedalWidth, editNoteHeight24 / 2));
      }else{
        tmpC.push_back(Rect(editStartX + buttonWidth * j, cY, buttonWidth, editNoteHeight24));
        tmpD.push_back(Rect(editStartX + buttonWidth * j, dY, buttonWidth, editNoteHeight24 / 2));
      }
    }

    edit24.click.push_back(tmpC);
    edit24.draw.push_back(tmpD);
  }

  for (int i = 0; i < 8; ++i) {
    splitButtons.push_back(Rect(splitStartX, splitStartY + i * 50, 40));
    splitPics.push_back(Texture(L"/11" + ToString(i)));
  }

  for (int i = 0; i < 3; ++i) {
    moveButtons.push_back(Rect(moveStartX + i * 50, moveStartY, 40));
    movePics.push_back(Texture(L"/13" + ToString(i)));
  }

  for (int i = 0; i < 4; ++i) {
    noteTypeButtons.push_back(Rect(noteTypeStartX, noteTypeStartY + i * 50, 40));
    noteTypePics.push_back(Texture(L"/12" + ToString(i)));
  }
  
  importButton = Rect(splitStartX + 100, splitStartY, 40);
  importPic = Texture(L"/160");

  saveButton = Rect(splitStartX + 100,splitStartY + 50,40);
  savePic = Texture(L"/161");

  //first measure
  measures.resize(1);

  f20 = Font(20);
  f10 = Font(10);

  jumpGUI = GUI(GUIStyle::Default);
  jumpGUI.setTitle(L"移動先の小節");
  jumpGUI.addln(L"targetMeasure", GUITextField::Create(4));
  jumpGUI.add(L"ok", GUIButton::Create(L"移動"));
  jumpGUI.add(L"cancel", GUIButton::Create(L"キャンセル"));
  jumpGUI.setCenter(Point(Window::Width()/2,Window::Height()/2));
  jumpGUI.show(false);

  normalGUI = GUI(GUIStyle::Default);
  normalGUI.setTitle(L"普通 or Ex");
  normalGUI.add(GUIText::Create(L"幅"));
  normalGUI.add(L"width", GUITextField::Create(3));
  normalGUI.addln(L"isEx", GUIToggleSwitch::Create(L"Ex", L"Ex", false));
  normalGUI.add(L"ok", GUIButton::Create(L"決定"));
  normalGUI.add(L"cancel", GUIButton::Create(L"キャンセル"));
  normalGUI.setCenter(Point(Window::Width() / 2, Window::Height() / 2));
  normalGUI.show(false);

  slideGUI = GUI(GUIStyle::Default);
  slideGUI.setTitle(L"スライド");
  slideGUI.add(GUIText::Create(L"幅(始点)"));
  slideGUI.add(L"startWidth", GUITextField::Create(3));
  slideGUI.addln(L"isTap", GUIToggleSwitch::Create(L"タップ判定なし", L"タップ判定なし", false));
  slideGUI.add(GUIText::Create(L"終点レーン"));
  slideGUI.add(L"endLane",GUITextField::Create(3));
  slideGUI.add(GUIText::Create(L"幅(終点)"));
  slideGUI.addln(L"endWidth", GUITextField::Create(3));
  slideGUI.add(L"split", GUITextField::Create(3));
  slideGUI.add(GUIText::Create(L"分で"));
  slideGUI.add(L"length", GUITextField::Create(4));
  slideGUI.addln(GUIText::Create(L"拍"));
  slideGUI.add(L"ok", GUIButton::Create(L"決定"));
  slideGUI.add(L"cancel", GUIButton::Create(L"キャンセル"));
  slideGUI.setCenter(Point(Window::Width() / 2, Window::Height() / 2));
  slideGUI.show(false);

  flickGUI = GUI(GUIStyle::Default);
  flickGUI.setTitle(L"フリック");
  flickGUI.add(GUIText::Create(L"幅"));
  flickGUI.addln(L"width", GUITextField::Create(3));
  flickGUI.add(L"ok", GUIButton::Create(L"決定"));
  flickGUI.add(L"cancel", GUIButton::Create(L"キャンセル"));
  flickGUI.setCenter(Point(Window::Width() / 2, Window::Height() / 2));
  flickGUI.show(false);

  pedalGUI = GUI(GUIStyle::Default);
  pedalGUI.setTitle(L"ペダルノーツ");
  pedalGUI.add(L"isLong", GUIToggleSwitch::Create(L"長押し", L"長押し", false));
  pedalGUI.add(L"split", GUITextField::Create(3, false));
  pedalGUI.add(GUIText::Create(L"分で"));
  pedalGUI.add(L"length", GUITextField::Create(4, false));
  pedalGUI.addln(GUIText::Create(L"拍"));
  pedalGUI.add(L"ok", GUIButton::Create(L"決定"));
  pedalGUI.add(L"cancel", GUIButton::Create(L"キャンセル"));
  pedalGUI.setCenter(Point(Window::Width() / 2, Window::Height() / 2));
  pedalGUI.show(false);

  optionGUI = GUI(GUIStyle::Default);
  optionGUI.setTitle(L"オプション");
  optionGUI.add(L"isBpm", GUICheckBox::Create({ L"BPM変化" }));
  optionGUI.add(GUIText::Create(L"BPM:"));
  optionGUI.addln(L"bpm", GUITextField::Create(5, false));
  optionGUI.add(L"isBeat", GUICheckBox::Create({ L"拍変化" }));
  optionGUI.add(L"beatSplit", GUITextField::Create(5, false));
  optionGUI.add(GUIText::Create(L"分の"));
  optionGUI.add(L"beat", GUITextField::Create(5, false));
  optionGUI.addln(GUIText::Create(L"拍子"));
  optionGUI.add(L"isStop", GUICheckBox::Create({ L"停止" }));
  optionGUI.add(L"stopSplit", GUITextField::Create(5, false));
  optionGUI.add(GUIText::Create(L"分で"));
  optionGUI.add(L"stopLength", GUITextField::Create(5, false));
  optionGUI.addln(GUIText::Create(L"拍"));
  optionGUI.add(L"ok", GUIButton::Create(L"決定"));
  optionGUI.add(L"cancel", GUIButton::Create(L"キャンセル"));
  optionGUI.setCenter(Point(Window::Width() / 2, Window::Height() / 2));
  optionGUI.show(false);
  
  importGUI = GUI(GUIStyle::Default);
  importGUI.setTitle(L"読み込み");
  importGUI.add(GUIText::Create(L"パス(相対パス)"));
  importGUI.addln(L"path", GUITextField::Create(20));
  importGUI.add(L"ok", GUIButton::Create(L"決定"));
  importGUI.add(L"cancel", GUIButton::Create(L"キャンセル"));
  importGUI.setCenter(Point(Window::Width() / 2, Window::Height() / 2));
  importGUI.show(false);

  saveGUI = GUI(GUIStyle::Default);
  saveGUI.setTitle(L"保存");
  saveGUI.add(GUIText::Create(L"パス(相対パス)"));
  saveGUI.addln(L"path", GUITextField::Create(20));
  saveGUI.add(L"ok", GUIButton::Create(L"決定"));
  saveGUI.add(L"cancel", GUIButton::Create(L"キャンセル"));
  saveGUI.setCenter(Point(Window::Width() / 2, Window::Height() / 2));
  saveGUI.show(false);

  while (System::Update()){
    if (jumpFlag) {
      if (jumpGUI.button(L"ok").pushed) {
        int num = Parse<int>(jumpGUI.textField(L"targetMeasure").text);
        if (num > 0) {
          jumpMeasure(num - 1);
        }
        jumpGUI.show(false);
        jumpFlag = false;
      }

      if (jumpGUI.button(L"cancel").pushed) {
        jumpGUI.show(false);
        jumpFlag = false;
      }
    }
    else if (normalFlag) {
      if (normalGUI.button(L"ok").pushed) {
        auto &cN = measures[currentMeasure].normalNotes;

        //widthの範囲チェック
        int width = ((16 - currentX) < Parse<int>(normalGUI.textField(L"width").text)) ? (16 - currentX) : Parse<int>(normalGUI.textField(L"width").text);
        if (!width) width = 1;

        cN.push_back(NormalNote(currentSplit,currentY,currentX,width,normalGUI.toggleSwitch(L"isEx").isRight));

        /*
        printf("--nN--\n");
        printf("split:%d\n", cN[cN.size() - 1].split);
        printf("Y:%d\n", cN[cN.size() - 1].y);
        printf("X:%d\n", cN[cN.size() - 1].x);
        printf("Width:%d\n", cN[cN.size() - 1].width);
        printf("isEx:%d\n", cN[cN.size() - 1].isEx);
        printf("------\n");
        */

        normalFlag = false;
        normalGUI.show(false);
      }

      if (normalGUI.button(L"cancel").pushed) {
        normalFlag = false;
        normalGUI.show(false);
      }
    }
    else if (slideFlag) {
      if (slideGUI.button(L"ok").pushed) {
        auto &cS = measures[currentMeasure].slideNotes;

        //endLaneの範囲チェック
        int endLane = Parse<int>(slideGUI.textField(L"endLane").text) - 1;
        if (endLane < 0) {
          endLane = 0;
        }
        else if (15 < endLane) {
          endLane = 15;
        }

        //startWidthの範囲チェック
        int startWidth = ((16 - currentX) < Parse<int>(slideGUI.textField(L"startWidth").text)) ? (16 - currentX) : Parse<int>(slideGUI.textField(L"startWidth").text);
        if (!startWidth) startWidth = 1;

        //endWidthの範囲チェック
        int endWidth = ((16 - endLane) < Parse<int>(slideGUI.textField(L"endWidth").text)) ? (16 - endLane) : Parse<int>(slideGUI.textField(L"endWidth").text);
        if (!endWidth) endWidth = 1;

        //lsplitの範囲チェック
        int lsplit = Parse<int>(slideGUI.textField(L"split").text);
        lsplit = (lsplit < 1) ? currentSplit : lsplit;

        //lengthの範囲チェック
        int length = Parse<int>(slideGUI.textField(L"length").text);
        length = (length < 1) ? 1 : length;

        cS.push_back(SlideNote(currentSplit,currentY,currentX,endLane,startWidth, endWidth,lsplit, length,!slideGUI.toggleSwitch(L"isTap").isRight));

        /*
        printf("--sN--\n");
        printf("split:%d\n", cS[cS.size() - 1].split);
        printf("Y:%d\n", cS[cS.size() - 1].y);
        printf("sX:%d\n", cS[cS.size() - 1].startX);
        printf("eX:%d\n", cS[cS.size() - 1].endX);
        printf("sWidth:%d\n", cS[cS.size() - 1].startWidth);
        printf("eWidth:%d\n", cS[cS.size() - 1].endWidth);
        printf("lSplit:%d\n", cS[cS.size() - 1].longSplit);
        printf("length:%d\n", cS[cS.size() - 1].length);
        printf("isTap:%d\n", cS[cS.size() - 1].isTap);
        printf("------\n");
        */

        slideFlag = false;
        slideGUI.show(false);
      }

      if (slideGUI.button(L"cancel").pushed) {
        slideFlag = false;
        slideGUI.show(false);
      }

    }
    else if (flickFlag) {
      if (flickGUI.button(L"ok").pushed) {
        auto &cF = measures[currentMeasure].flickNotes;

        //widthの範囲チェック
        int width = ((16 - currentX) < Parse<int>(flickGUI.textField(L"width").text)) ? (16 - currentX) : Parse<int>(flickGUI.textField(L"width").text);
        if (!width) width = 1;

        cF.push_back(FlickNote(currentSplit, currentY, currentX, width));

        /*
        printf("--fN--\n");
        printf("split:%d\n", cF[cF.size() - 1].split);
        printf("Y:%d\n", cF[cF.size() - 1].y);
        printf("X:%d\n", cF[cF.size() - 1].x);
        printf("Width:%d\n", cF[cF.size() - 1].width);
        printf("------\n");
        */

        flickFlag = false;
        flickGUI.show(false);
      }

      if (flickGUI.button(L"cancel").pushed) {
        flickFlag = false;
        flickGUI.show(false);
      }
    }
    else if (pedalFlag) {
      if (pedalGUI.toggleSwitch(L"isLong").isRight) {
        pedalGUI.textField(L"split").enabled = true;
        pedalGUI.textField(L"length").enabled = true;
      }
      else {
        pedalGUI.textField(L"split").enabled = false;
        pedalGUI.textField(L"length").enabled = false;
      }

      if (pedalGUI.button(L"ok").pushed) {
        auto &cP = measures[currentMeasure].pedalNotes;

        if (pedalGUI.toggleSwitch(L"isLong").isRight) {
          //lsplitの範囲チェック
          int lsplit = Parse<int>(pedalGUI.textField(L"split").text);
          lsplit = (lsplit < 1) ? currentSplit : lsplit;

          //lengthの範囲チェック
          int length = Parse<int>(pedalGUI.textField(L"length").text);
          length = (length < 1) ? 1 : length;

          cP.push_back(PedalNote(currentSplit, currentY, lsplit, length));
        }
        else {
          cP.push_back(PedalNote(currentSplit, currentY, 0, 0));
        }

        /*
        printf("--sN--\n");
        printf("split:%d\n", cP[cP.size() - 1].split);
        printf("Y:%d\n", cP[cP.size() - 1].y);
        printf("lSplit:%d\n", cP[cP.size() - 1].longSplit);
        printf("length:%d\n", cP[cP.size() - 1].length);
        printf("------\n");
        */

        pedalFlag = false;
        pedalGUI.show(false);
      }

      if (pedalGUI.button(L"cancel").pushed) {
        pedalFlag = false;
        pedalGUI.show(false);
      }
    }
    else if (optionFlag) {

      if (optionGUI.checkBox(L"isBpm").hasChanged) {
        if (optionGUI.checkBox(L"isBpm").checked(0)) {
          optionGUI.textField(L"bpm").enabled = true;
        }
        else {
          optionGUI.textField(L"bpm").enabled = false;
        }
      }

      if (optionGUI.checkBox(L"isBeat").hasChanged) {
        if (optionGUI.checkBox(L"isBeat").checked(0)) {
          optionGUI.textField(L"beatSplit").enabled = true;
          optionGUI.textField(L"beat").enabled = true;
        }
        else {
          optionGUI.textField(L"beatSplit").enabled = false;
          optionGUI.textField(L"beat").enabled = false;
        }
      }

      if (optionGUI.checkBox(L"isStop").hasChanged) {
        if (optionGUI.checkBox(L"isStop").checked(0)) {
          optionGUI.textField(L"stopSplit").enabled = true;
          optionGUI.textField(L"stopLength").enabled = true;
        }
        else {
          optionGUI.textField(L"stopSplit").enabled = false;
          optionGUI.textField(L"stopLength").enabled = false;
        }
      }

      if (optionGUI.button(L"ok").pushed) {
        bool  isBpm = optionGUI.checkBox(L"isBpm").checked(0);
        bool isBeat = optionGUI.checkBox(L"isBeat").checked(0);
        bool isStop = optionGUI.checkBox(L"isStop").checked(0);

        double bpm = 0.0;
        int beatSplit = 0, beat = 0;
        int stopSplit = 0, stopLength = 0;

        if (isBpm) {
          bpm = Parse<double>(optionGUI.textField(L"bpm").text);
        }

        if (isBeat) {
          beatSplit = Parse<double>(optionGUI.textField(L"beatSplit").text);
          beat = Parse<double>(optionGUI.textField(L"beat").text);
        }

        if (isStop) {
          stopSplit = Parse<double>(optionGUI.textField(L"stopSplit").text);
          stopLength = Parse<double>(optionGUI.textField(L"stopLength").text);
        }

        
        measures[currentMeasure].options.push_back(Option(currentSplit, currentY, isBpm, isBeat, isStop, bpm, beatSplit, beat, stopSplit, stopLength));
        
        optionFlag = false;
        optionGUI.show(false);
      }

      if (optionGUI.button(L"cancel").pushed) {
        optionFlag = false;
        optionGUI.show(false);
      }
    }
    else if(importFlag) {
      if (importGUI.button(L"ok").pushed) {
        String path = importGUI.textField(L"path").text;
        import(path);

        importFlag = false;
        importGUI.show(false);
      }

      if (importGUI.button(L"cancel").pushed) {
        importFlag = false;
        importGUI.show(false);
      }
    }
    else if (saveFlag) {
      if (saveGUI.button(L"ok").pushed) {
        String path = saveGUI.textField(L"path").text;
        
        saveFile(path);

        saveFlag = false;
        saveGUI.show(false);
      }

      if (saveGUI.button(L"cancel").pushed) {
        saveFlag = false;
        saveGUI.show(false);
      }
    }
    else {
      update();
    }
    draw();
  }

#ifdef _DEBUG
  fclose(out);
  fclose(in);
  FreeConsole();
#endif // _DEBUG

}

void update() {
  if (Input::KeyRight.clicked || moveButtons[1].leftClicked) {
    nextMeasure();
  }
  if (Input::KeyLeft.clicked  || moveButtons[0].leftClicked) {
    prevMeasure();
  }
  if (moveButtons[2].leftClicked) {
    jumpGUI.textField(L"targetMeasure").setText(ToString(currentMeasure + 1));
    jumpGUI.show(true);
    jumpFlag = true;
  }

  for (int i = 0; i < 8; ++i) {
    if (splitButtons[i].leftClicked) {
      split = splitList[i];
    }
  }

  for (int i = 0; i < noteTypeButtons.size();++i) {
    if (noteTypeButtons[i].leftClicked) {
      noteType = i;
    }
  }

  if (importButton.leftClicked) {
    importGUI.show(true);
    importFlag = true;
  }

  if (saveButton.leftClicked) {
    saveGUI.show(true);
    saveFlag = true;
  }

  //スライダーのノーツの追加処理
  if (split % 3 == 0) {
    for (int i = 0; i < 24; ++i) {
      for (int j = 0; j < 16; ++j) {
        if (edit24.click[i][j].leftReleased) {
          currentX = j;
          currentSplit = split;
          currentY = i / (24 / split);

          switch (noteType){
          case 0://normal or Ex
            normalFlag = true;
            normalGUI.show(true);
            break;
          case 1://flick
            flickFlag = true;
            flickGUI.show(true);
            break;
          case 2://slide
            slideFlag = true;
            slideGUI.show(true);
            break;
          }
        }
      }
    }
  }
  else {
    for (int i = 0; i < 32; ++i) {
      for (int j = 0; j < 16; ++j) {
        if (edit32.click[i][j].leftReleased) {
          currentX = j;
          currentSplit = split;
          currentY = i / (32 / split);

          switch (noteType) {
          case 0://normal or Ex
            normalFlag = true;
            normalGUI.show(true);
            break;
          case 1://flick
            flickFlag = true;
            flickGUI.show(true);
            break;
          case 2://slide
            slideFlag = true;
            slideGUI.show(true);
            break;
          }
        }
      }
    } 
  }
  
  //ペダルのノーツ追加処理
  if (noteType != 3) {
    if (split % 3 == 0) {
      for (int i = 0; i < 24; ++i) {
        if (edit24.click[i][16].leftReleased) {
          currentSplit = split;
          currentY = i / (24 / split);

          pedalFlag = true;
          pedalGUI.show(true);
        }
      }
    }
    else {
      for (int i = 0; i < 32; ++i) {
        if (edit32.click[i][16].leftReleased) {
          currentSplit = split;
          currentY = i / (32 / split);

          pedalFlag = true;
          pedalGUI.show(true);
        }
      }
    }
  }

  //オプションの追加処理
  if (noteType == 3) {
    if (split % 3 == 0) {
      for (int i = 0; i < 24; ++i) {
        for (int j = 0; j < 17; ++j) {
          if (edit24.click[i][j].leftReleased) {
            currentSplit = split;
            currentY = i/(24/split);
            optionFlag = true;
            optionGUI.show(true);
          }
        }
      }
    }
    else {
      for (int i = 0; i < 32; ++i) {
        for (int j = 0; j < 17; ++j) {
          if (edit32.click[i][j].leftReleased) {
            currentSplit = split;
            currentY = i / (32 / split);
            optionFlag = true;
            optionGUI.show(true);
          }
        }
      }
    }
  }


  //スライダーの単押しノーツの削除
  if (noteType == 0) {
    auto &normalNotes = measures[currentMeasure].normalNotes;
    for (int i = normalNotes.size() - 1; i >= 0; --i) {
      auto &cN = normalNotes[i];
      bool isErase = false;

      if (cN.split % 3 == 0) {
        int index = cN.y * (24 / cN.split);
        for (int j = cN.x; j < cN.x + cN.width; ++j) {
          if (edit24.draw[index][j].rightClicked) {
            normalNotes.erase(normalNotes.begin() + i);
            isErase = true;
            break;
          }
        }
      }
      else {
        int index = cN.y * (32 / cN.split);
        for (int j = cN.x; j < cN.x + cN.width; ++j) {
          if (edit32.draw[index][j].rightClicked) {
            normalNotes.erase(normalNotes.begin() + i);
            isErase = true;
            break;
          }
        }
      }

      //一気に何個も消えないようにする
      if (isErase) {
        break;
      }
    }
  }

  //スライダーのフリックノーツの削除
  if (noteType == 1) {
    auto &flickNotes = measures[currentMeasure].flickNotes;
    for (int i = flickNotes.size() - 1; i >= 0; --i) {
      auto &cF = flickNotes[i];
      bool isErase = false;

      if (cF.split % 3 == 0) {
        int index = cF.y * (24 / cF.split);
        for (int j = cF.x; j < cF.x + cF.width; ++j) {
          if (edit24.draw[index][j].rightClicked) {
            flickNotes.erase(flickNotes.begin() + i);
            isErase = true;
            break;
          }
        }
      }
      else {
        int index = cF.y * (32 / cF.split);
        for (int j = cF.x; j < cF.x + cF.width; ++j) {
          if (edit32.draw[index][j].rightClicked) {
            flickNotes.erase(flickNotes.begin() + i);
            isErase = true;
            break;
          }
        }
      }

      //一気に何個も消えないようにする
      if (isErase) {
        break;
      }
    }
  }
  
  //スライダーのスライドノーツの削除
  if (noteType == 2) {
    for (int i = currentMeasure; i >=0; --i) {
      auto &cS = measures[i].slideNotes;
      auto isErase = false;

      for (int j = cS.size() -1; j >= 0;--j) {

        double start = i + ((double)cS[j].y / cS[j].split);
        double length = ((double)cS[j].length / cS[j].longSplit);
        double end = start + length;

        if (start < currentMeasure + 1 && currentMeasure < end) {
          int endY = editStartX + (editHeight - (end - currentMeasure)*editHeight);
          int startY = editStartY + (editHeight - (start - currentMeasure)*editHeight);

          if (Quad({ laneStartXs[cS[j].endX], endY }, { laneStartXs[cS[j].endX + cS[j].endWidth], endY }, { laneStartXs[cS[j].startX + cS[j].startWidth],startY }, { laneStartXs[cS[j].startX],startY }).rightClicked) {
            cS.erase(cS.begin() + j);
            isErase = true;
            break;
          }
        }
      }

      if (isErase) {
        break;
      }
    }
  }

  //ペダルのノーツの削除
  for (int i = 0; i <= currentMeasure; ++i) {
    auto &cP = measures[i].pedalNotes;
    bool isErase = false;
    for (int j = cP.size() - 1; j >= 0; --j) {

      //長押し部分の描画
      if (cP[j].longSplit != 0 && cP[j].length != 0) {
        double start = i + ((double)cP[j].y / cP[j].split);
        double length = ((double)cP[j].length / cP[j].longSplit);
        double end = start + length;

        if (start < currentMeasure + 1 && currentMeasure < end) {
          int endY = editStartX + (editHeight - (end - currentMeasure)*editHeight);
          int startY = editStartY + (editHeight - (start - currentMeasure)*editHeight);

          if (Quad({ laneStartXs[16], endY }, { laneStartXs[16] + pedalWidth, endY }, { laneStartXs[16] + pedalWidth,startY }, { laneStartXs[16] ,startY }).rightClicked) {
            cP.erase(cP.begin() + j);
            isErase = true;
            break;
          }
        }
      }

      //現在の小節なら始点ノーツ表示
      if (i == currentMeasure) {
        if (cP[j].split % 3 == 0) {
          int index = cP[j].y * (24 / cP[j].split);
          if (edit24.draw[index][16].rightClicked) {
            cP.erase(cP.begin() + j);
            isErase = true;
            break;
          }
        }
        else {
          int index = cP[j].y * (32 / cP[j].split);
          if (edit32.draw[index][16].rightClicked) {
            cP.erase(cP.begin() + j);
            isErase = true;
            break;
          }
        }
      }
    }

    if (isErase) {
      break;
    }
  }

  //オプションの削除
  for (int i = 0; i < measures[currentMeasure].options.size(); ++i) {
    auto &cO = measures[currentMeasure].options[i];

    Circle c;
    if (cO.split % 3 == 0) {
      c = Circle(editStartX + editWidth, editStartY + edit24.draw[(24 / cO.split) * cO.y][0].y, 10);
    }
    else {
      c = Circle(editStartX + editWidth, editStartY + edit32.draw[(32 / cO.split) * cO.y][0].y, 10);
    }

    if (c.rightClicked) {
      measures[currentMeasure].options.erase(measures[currentMeasure].options.begin() + i);
      break;
    }
  }
}

void draw() {
  drawEdit(editStartX,editStartY);
}

void drawEdit(int sX, int sY) {
  //スライダーの単押しノーツの描画
  for (const auto &i : measures[currentMeasure].normalNotes) {
    Color noteColor = (i.isEx) ? Color(255,255,0,128) : Color(255,0,0,128);

    if (i.split % 3 == 0) {
      int index = i.y * (24 / i.split);
      for (int j = i.x; j < i.x + i.width; ++j) {
        edit24.draw[index][j].draw(noteColor);
      }
    }
    else {
      for (int j = i.x; j < i.x + i.width; ++j) {
        int index = i.y * (32 / i.split);
        edit32.draw[index][j].draw(noteColor);
      }
    }
  }

  //スライダーのフリックノーツの描画
  for (const auto &i : measures[currentMeasure].flickNotes) {

    if (i.split % 3 == 0) {
      int index = i.y * (24 / i.split);
      for (int j = i.x; j < i.x + i.width; ++j) {
        edit24.draw[index][j].draw(Color(0, 255, 0, 128));
      }
    }
    else {
      for (int j = i.x; j < i.x + i.width; ++j) {
        int index = i.y * (32 / i.split);
        edit32.draw[index][j].draw(Color(0, 255, 0, 128));
      }
    }
  }

  //スライダーのスライドノーツの描画
  //最初から現在の小節まで
  for (int i = 0; i <= currentMeasure; ++i) {
    auto &cS = measures[i].slideNotes;

    for (const auto &j: cS) {

      double start = i + ((double)j.y / j.split);
      double length = ((double)j.length / j.longSplit);
      double end = start + length;

      if (start < currentMeasure + 1 && currentMeasure < end) {
        int endY = sY + (editHeight - (end - currentMeasure)*editHeight);
        int startY = sY + (editHeight - (start - currentMeasure)*editHeight);

        Quad({laneStartXs[j.endX], endY}, {laneStartXs[j.endX + j.endWidth], endY },
        { laneStartXs[j.startX + j.startWidth],startY }, {laneStartXs[j.startX],startY}).draw(Color(25, 232, 232,64));

        //タップ判定ありなら始点ノーツ表示
        if (j.isTap && i == currentMeasure) {
          if (j.split % 3 == 0) {
            int index = j.y * (24 / j.split);
            for (int k = j.startX; k < j.startX + j.startWidth; ++k) {
              edit24.draw[index][k].draw(Color(0, 0, 255, 64));
            }
          }
          else {
            int index = j.y * (32 / j.split);
            for (int k = j.startX; k < j.startX + j.startWidth; ++k) {
              edit32.draw[index][k].draw(Color(0, 0, 255, 64));
            }
          }
        }
      }
      
    }
  }
  
  //ペダルのノーツの描画
  //最初から現在の小節まで
  for (int i = 0; i <= currentMeasure; ++i) {
    auto &cP = measures[i].pedalNotes;
    for (const auto &j : cP) {

      //長押し部分の描画
      if (j.longSplit != 0 && j.length != 0) {
        double start = i + ((double)j.y / j.split);
        double length = ((double)j.length / j.longSplit);
        double end = start + length;

        if (start < currentMeasure + 1 && currentMeasure < end) {
          int endY = sY + (editHeight - (end - currentMeasure)*editHeight);
          int startY = sY + (editHeight - (start - currentMeasure)*editHeight);

          Quad({ laneStartXs[16], endY }, { laneStartXs[16] + pedalWidth, endY },
          { laneStartXs[16] + pedalWidth,startY }, { laneStartXs[16] ,startY }).draw(Color(25, 232, 232, 64));
        }
      }

      //現在の小節なら始点ノーツ表示
      if (i == currentMeasure) {
        if (j.split % 3 == 0) {
          int index = j.y * (24 / j.split);
           edit24.draw[index][16].draw(Color(0, 0, 255, 64));
        }
        else {
          int index = j.y * (32 / j.split);
          edit32.draw[index][16].draw(Color(0, 0, 255, 64));
        }
      }
    }
  }


  //はみ出るスライドを隠す
  Quad({ sX,0 }, { sX + editWidth , 0 }, { sX + editWidth , sY }, { sX,sY }).draw(Color(40));
  Quad({ sX,sY + editHeight}, { sX + editWidth , sY + editHeight }, { sX + editWidth , Window::Height() }, { sX,Window::Height()}).draw(Color(40));

  //ボタン
  for (int i = 0; i < 8; ++i) {
    if (splitList[i] == split) {
      splitButtons[i].draw(Color(150));
    }
    splitButtons[i](splitPics[i]).draw();
  }

  for (int i = 0; i < 3; ++i) {
    moveButtons[i](movePics[i]).draw();
  }

  f20(currentMeasure + 1).draw(moveStartX - 80, moveStartY);

  for (int i = 0; i < noteTypeButtons.size(); ++i) {
    if (i == noteType) {
      noteTypeButtons[i].draw(Color(150));
    }
    noteTypeButtons[i](noteTypePics[i]).draw();
  }

  importButton(importPic).draw();
  saveButton(savePic).draw();

  //横線
  if (split % 3 == 0) {
    for (int i = 0; i < 24; i += (24 / split)) {
      Color color = i % (24 / 3) == 0 ? Color(255, 0, 0) : Color(255);
      Line(sX, sY + i*editNoteHeight24, sX + editWidth, sY + i*editNoteHeight24).draw(color);
    }
  } else {
    for (int i = 0; i < 32; i += (32 / split)) {
      Color color = i % (32 / 4) == 0 ? Color(255, 0, 0) : Color(255);
      Line(sX, sY + i*editNoteHeight32, sX + editWidth, sY + i*editNoteHeight32).draw(color);
    }
  }

  //縦線
  for (int i = 0; i < 17; ++i) {
    Color color = (i < 13 && i % 4 == 0) ? Color(255, 0, 0) : Color(255);
    Line(sX + buttonWidth * i, sY, sX + buttonWidth * i, sY + editHeight).draw(color);
  }

  
  //topLine
  Line(sX, sY,sX + editWidth,sY).draw();
  //bottomLine
  Line(sX, sY + editHeight, sX + editWidth, sY + editHeight).draw();
  //leftLine
  Line(sX, sY, sX, sY + editHeight).draw();
  //rightLine
  Line(sX + editWidth, sY, sX + editWidth, sY + editHeight).draw();


  //情報表示したいから一番最後に
  //オプションの描画
  for (int i = 0; i < measures[currentMeasure].options.size(); ++i) {
    auto &cO = measures[currentMeasure].options[i];
    Circle c;
    if (cO.split % 3 == 0) {
      c = Circle(sX + editWidth, sY + edit24.draw[(24 / cO.split) * cO.y][0].y, 10);
    }
    else {
      c = Circle(sX + editWidth, sY + edit32.draw[(32 / cO.split) * cO.y][0].y, 10);
    }

    c.draw(Color(255, 255, 0, 128));

    if (c.mouseOver) {
      Rect(c.x, c.y, 200, 50).draw(Color(100, 100, 100));

      if (cO.isBpm) {
        f10(L"BPM : " + ToString(cO.bpm)).draw(c.x + 20, c.y);
      }
      if (cO.isBeat) {
        f10(L"拍 : " + ToString(cO.beatSplit) + L"分の" + ToString(cO.beat) + L"拍").draw(c.x + 20, c.y + 15);
      }
      if (cO.isStop) {
        f10(L"停止 : " + ToString(cO.stopSplit) + L"分で" + ToString(cO.stopLength) + L"拍").draw(c.x + 20, c.y + 30);
      }
    }
  }

}

void nextMeasure() {
  currentMeasure++;
  if (currentMeasure >= measures.size()) {
    //次の小節が存在しないとき
    measures.resize(currentMeasure + 1);
  }
}

void prevMeasure() {
  if (0 <= currentMeasure-1) {
    currentMeasure--;
  }
}

void jumpMeasure(int measure) {
  currentMeasure = measure;
  if (currentMeasure >= measures.size()) {
    //ジャンプ先の小節が存在しないとき
    measures.resize(currentMeasure + 1);
  }
}

int lcm(int n1,int n2) {
  if (n1 > n2) std::swap(n1, n2);
  int x = n1 * n2;
  int r = n2 % n1;
  while (r != 0) {
    n2 = n1;
    n1 = r;
    r = n2 % n1;
  }

  return x / n1;
}

void import(String path) {
  XMLReader reader(path);
  auto el = reader.root().firstChild();

  //ファイル開けなかったとき
  if (!reader.isValid()) {
    return;
  }

  fileOptions.clear();
  while (el.name() != L"NOTES") {
    fileOptions.push_back(FileOption(el.name(),el.text()));
    el = el.nextSibling();
  }

  std::vector<std::vector<String>> line;
  //小節
  for (auto &i : el.text().split(L'@')) {
    std::vector<String> tmp;
    //拍
    for (auto &j : i.split(L';')) {
      tmp.push_back(j);
    }
    line.push_back(tmp);
  }

  //最後の要素は最後の;の後だからない
  measures.resize(line.size());

  for (int i = 0; i < line.size() - 1; ++i) {
    for (int j = 0; j < line[i].size() - 1; ++j) {
      std::smatch results;
      std::string target = line[i][j].narrow();

      String optionStr = L"", noteStr = L"", pedalStr = L"";

      if (std::regex_match(target, results, 
        std::regex(R"( *((?:\|.+\|)*){0,1}((?:\{.+\})*){0,1}((?:\[.+\])*){0,1})"))) {
        optionStr =Widen(results[1].str());
        noteStr = Widen(results[2].str());
        pedalStr = Widen(results[3].str());
      }
      else {
        continue;
      }
      
      if (optionStr.length > 5) {
        Option tmp;

        auto options = optionStr.split(L'|');

        for (int k = 1; k < options.size(); k += 2) {
          int n[3];
          sscanf_s(options[k].narrow().c_str(), "%d,%d,%d", &n[0], &n[1], &n[2]);
          printf("%d %d %d\n", n[0], n[1], n[2]);

          if (n[0] == 1) {
            tmp.isStop = true;
            tmp.stopSplit = n[1];
            tmp.stopLength = n[2];
          }

          if (n[0] == 2) {
            tmp.isBpm = true;
            tmp.bpm = n[1];
          }

          if (n[0] == 3) {
            tmp.isBeat = true;
            tmp.beatSplit = n[1];
            tmp.beat = n[2];
          }

          if (line[i].size() - 1 == 48) {
            if (j % (48 / 16) == 0) {
              tmp.split = 16;
              tmp.y = j / (48 / 16);
            }
            else {
              tmp.split = 24;
              tmp.y = j / (48 / 24);
            }
          }
          else if (line[i].size() - 1 == 96) {
            if (j % (96 / 32) == 0) {
              tmp.split = 32;
              tmp.y = j / (96 / 32);
            }
            else {
              tmp.split = 24;
              tmp.y = j / (96 / 24);
            }
          }
          else {
            tmp.split = line[i].size() - 1;
            tmp.y = j;
          }
        }

        measures[i].options.push_back(tmp);
      }

      auto notes = noteStr.split(L'{');
      for (int k = 1; k < notes.size(); ++k) {

        int n[7];
        sscanf_s(notes[k].narrow().c_str(), "%d,%d,%d,%d,%d,%d,%d}", &n[0], &n[1], &n[2], &n[3], &n[4], &n[5], &n[6]);
        printf("%d %d %d %d %d %d %d\n", n[0], n[1], n[2], n[3], n[4], n[5], n[6]);


        if (n[1] == 1) {
          NormalNote tmp;
          tmp.x = n[0] - 1;
          tmp.width = n[2];
          tmp.isEx = n[3];

          if (line[i].size() - 1 == 48) {
            if (j % (48 / 16) == 0) {
              tmp.split = 16;
              tmp.y = j / (48 / 16);
            }
            else {
              tmp.split = 24;
              tmp.y = j / (48 / 24);
            }
          }
          else if (line[i].size() - 1 == 96) {
            if (j % (96 / 32) == 0) {
              tmp.split = 32;
              tmp.y = j / (96 / 32);
            }
            else {
              tmp.split = 24;
              tmp.y = j / (96 / 24);
            }
          }
          else {
            tmp.split = line[i].size() - 1;
            tmp.y = j;
          }

          measures[i].normalNotes.push_back(tmp);
        }
        else if (n[1] == 2 || n[1] == 3) {
          SlideNote tmp;
          tmp.startX = n[0] - 1;
          tmp.startWidth = n[2];
          tmp.endWidth = n[3];
          tmp.endX = n[4] - 1;
          tmp.longSplit = n[5];
          tmp.length = n[6];

          tmp.isTap = (n[1] == 2) ? true : false;

          if (line[i].size() - 1 == 48) {
            if (j % (48 / 16) == 0) {
              tmp.split = 16;
              tmp.y = j / (48 / 16);
            }
            else {
              tmp.split = 24;
              tmp.y = j / (48 / 24);
            }
          }
          else if (line[i].size() - 1 == 96) {
            if (j % (96 / 32) == 0) {
              tmp.split = 32;
              tmp.y = j / (96 / 32);
            }
            else {
              tmp.split = 24;
              tmp.y = j / (96 / 24);
            }
          }
          else {
            tmp.split = line[i].size() - 1;
            tmp.y = j;

          }
          measures[i].slideNotes.push_back(tmp);
        }
        else if (n[1] == 4) {
          FlickNote tmp;

          tmp.x = n[0] - 1;
          tmp.width = n[2];

          if (line[i].size() - 1 == 48) {
            if (j % (48 / 16) == 0) {
              tmp.split = 16;
              tmp.y = j / (48 / 16);
            }
            else {
              tmp.split = 24;
              tmp.y = j / (48 / 24);
            }
          }
          else if (line[i].size() - 1 == 96) {
            if (j % (96 / 32) == 0) {
              tmp.split = 32;
              tmp.y = j / (96 / 32);
            }
            else {
              tmp.split = 24;
              tmp.y = j / (96 / 24);
            }
          }
          else {
            tmp.split = line[i].size() - 1;
            tmp.y = j;
          }

          measures[i].flickNotes.push_back(tmp);
        }
      }

      if (pedalStr.length > 5) {
        PedalNote tmp;

        int n[3];

        sscanf_s(pedalStr.narrow().c_str(), "[%d,%d,%d]", &n[0], &n[1], &n[2]);
        printf("%d %d %d\n", n[0], n[1], n[2]);

        if (n[0] == 1) {
          tmp.longSplit = 0;
          tmp.length = 0;
        }
        else {
          tmp.longSplit = n[1];
          tmp.length = n[2];
        }

        if (line[i].size() - 1 == 48) {
          if (j % (48 / 16) == 0) {
            tmp.split = 16;
            tmp.y = j / (48 / 16);
          }
          else {
            tmp.split = 24;
            tmp.y = j / (48 / 24);
          }
        }
        else if (line[i].size() - 1 == 96) {
          if (j % (96 / 32) == 0) {
            tmp.split = 32;
            tmp.y = j / (96 / 32);
          }
          else {
            tmp.split = 24;
            tmp.y = j / (96 / 24);
          }
        }
        else {
          tmp.split = line[i].size() - 1;
          tmp.y = j;
        }

        measures[i].pedalNotes.push_back(tmp);
      }
    }
  }
}

void saveFile(String path) {
  TextWriter writer(path);
  if (writer.isOpened()) {

    writer.writeln(L"﻿<MusicData>");

    if (fileOptions.size() != 0) {
      for (const auto &i : fileOptions) {
        writer.writeln(L"  <" + i.name + L">" + i.text + L"</" + i.name + L">");
        
      }
    }
    else {
      for (const auto &i : optionNames) {
        writer.writeln(L"  <" + i + L">" + L"</" + i + L">");
      }
    }

    writer.writeln(L"\n  ﻿<NOTES>");

    for (int i = 0; i < measures.size(); ++i) {
      int max32 = 1, max24 = 1;
      for (const auto &j : measures[i].normalNotes) {
        if (j.split % 3 == 0) max24 = std::max<int>(j.split,max24);
        else max32 = std::max<int>(j.split, max32);
      }
      for (const auto &j : measures[i].flickNotes) {
        if (j.split % 3 == 0) max24 = std::max<int>(j.split, max24);
        else max32 = std::max<int>(j.split, max32);
      }
      for (const auto &j : measures[i].slideNotes) {
        if (j.split % 3 == 0) max24 = std::max<int>(j.split, max24);
        else max32 = std::max<int>(j.split, max32);
      }
      for (const auto &j : measures[i].pedalNotes) {
        if (j.split % 3 == 0) max24 = std::max<int>(j.split, max24);
        else max32 = std::max<int>(j.split, max32);
      }
      for (const auto &j : measures[i].options) {
        if (j.split % 3 == 0) max24 = std::max<int>(j.split, max24);
        else max32 = std::max<int>(j.split, max32);
      }

      int outputSplit = lcm(max24, max32);

      //行
      for (int j = 0; j < outputSplit; ++j) {
        writer.write(L"    ");

        //オプションの出力
        for (const auto &k : measures[i].options) {
          if (k.y * (outputSplit / k.split) == j) {
            if (k.isStop) {
              writer.write(L"|1," + ToString(k.stopSplit) + L"," + ToString(k.stopLength) + L"|");
            }
            if (k.isBpm) {
              writer.write(L"|2," + ToString(static_cast<int>(k.bpm)) + L",0|");
            }
            if (k.isBeat) {
              writer.write(L"|3," + ToString(k.beatSplit) + L"," + ToString(k.beat) + L"|");
            }

          }
        }

        //ノーツの出力
        std::array<std::vector<String>, 16> tmp;
        for (const auto &k : measures[i].normalNotes) {
          if (k.y * (outputSplit / k.split) == j) {
            tmp[k.x].push_back(L"{" + ToString(k.x + 1) + L",1,"+ ToString(k.width) + L"," + ((k.isEx)? L"1": L"0") + L",0,0,0}");
          }
        }
        for (const auto &k : measures[i].flickNotes) {
          if (k.y * (outputSplit / k.split) == j) {
            tmp[k.x].push_back(L"{" + ToString(k.x + 1) + L",4," + ToString(k.width) + L",0,0,0,0}");
          }
        }
        for (const auto &k : measures[i].slideNotes) {
          if (k.y * (outputSplit / k.split) == j) {
            tmp[k.startX].push_back(L"{" + ToString(k.startX + 1) + ((k.isTap)? L",2," : L",3,") + ToString(k.startWidth) + L"," + ToString(k.endWidth) + L"," +
              ToString(k.endX + 1) + L"," + ToString(k.longSplit) + L"," + ToString(k.length)  + L"}");
          }
        }

        for (const auto &k : tmp) {
          for(const auto &l: k){
            writer.write(l);
          }
        }

        //ペダルの出力
        for (const auto &k : measures[i].pedalNotes) {
          if (k.y * (outputSplit / k.split) == j) {
            if (k.longSplit == 0 || k.length == 0) {
              writer.write(L"[1,0,0]");
            }
            else {
              writer.write(L"[2," + ToString(k.longSplit) + L"," + ToString(k.length) + L"]");
            }
            
          }
        }


        if (j == outputSplit - 1) writer.write(L";");
        else writer.write(L";\n");
      }

      writer.writeln(L"@");
    }

    writer.writeln(L"  ﻿</NOTES>");
    writer.write(L"﻿</MusicData>");
  }
}
