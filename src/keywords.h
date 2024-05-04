#ifndef KUMIR_KEYWORDS_H
#define KUMIR_KEYWORDS_H

#define LANG_RU

#ifdef LANG_EN

// Control keywords

#define KEYWORD_IF       "if"
#define KEYWORD_THEN     "then"
#define KEYWORD_ENDIF    "endif"
#define KEYWORD_LOOP     "loop while"
#define KEYWORD_ENDLOOP  "endloop"

#define KEYWORD_NOT      "not"
#define KEYWORD_AND      "and"
#define KEYWORD_OR       "or"

#define KEYWORD_EXIT     "exit"

// Robot keywords

#define KEYWORD_SETPOS   "goto"
#define KEYWORD_GO_UP    "go up"
#define KEYWORD_GO_DOWN  "go down"
#define KEYWORD_GO_LEFT  "go left"
#define KEYWORD_GO_RIGHT "go right"
#define KEYWORD_PAINT    "paint"

#define KEYWORD_CHECK_CLEAR  "free"
#define KEYWORD_CHECK_UP     "upwards"
#define KEYWORD_CHECK_DOWN   "downwards"
#define KEYWORD_CHECK_LEFT   "leftwards"
#define KEYWORD_CHECK_RIGHT  "rightwards"

#endif // LANG_EN

#ifdef LANG_RU

// Control keywords

#define KEYWORD_IF       "если"
#define KEYWORD_THEN     "то"
#define KEYWORD_ENDIF    "все"
#define KEYWORD_LOOP     "нц пока"
#define KEYWORD_ENDLOOP  "кц"

#define KEYWORD_NOT      "не"
#define KEYWORD_AND      "и"
#define KEYWORD_OR       "или"

#define KEYWORD_EXIT     "конец"

// Robot keywords

#define KEYWORD_SETPOS   "переместить"
#define KEYWORD_GO_UP    "вверх"
#define KEYWORD_GO_DOWN  "вниз"
#define KEYWORD_GO_LEFT  "влево"
#define KEYWORD_GO_RIGHT "вправо"
#define KEYWORD_PAINT    "закрасить"

#define KEYWORD_CHECK_CLEAR  "свободно"
#define KEYWORD_CHECK_UP     "сверху"
#define KEYWORD_CHECK_DOWN   "снизу"
#define KEYWORD_CHECK_LEFT   "слева"
#define KEYWORD_CHECK_RIGHT  "справа"

#endif // LANG_RU

#define KEYWORD_CHECK_UP_FULL     KEYWORD_CHECK_UP" "KEYWORD_CHECK_CLEAR
#define KEYWORD_CHECK_DOWN_FULL   KEYWORD_CHECK_DOWN" "KEYWORD_CHECK_CLEAR
#define KEYWORD_CHECK_LEFT_FULL   KEYWORD_CHECK_LEFT" "KEYWORD_CHECK_CLEAR
#define KEYWORD_CHECK_RIGHT_FULL  KEYWORD_CHECK_RIGHT" "KEYWORD_CHECK_CLEAR

#define KEYWORD_NOT_CHECK_UP_FULL     KEYWORD_NOT" "KEYWORD_CHECK_UP" "KEYWORD_CHECK_CLEAR
#define KEYWORD_NOT_CHECK_DOWN_FULL   KEYWORD_NOT" "KEYWORD_CHECK_DOWN" "KEYWORD_CHECK_CLEAR
#define KEYWORD_NOT_CHECK_LEFT_FULL   KEYWORD_NOT" "KEYWORD_CHECK_LEFT" "KEYWORD_CHECK_CLEAR
#define KEYWORD_NOT_CHECK_RIGHT_FULL  KEYWORD_NOT" "KEYWORD_CHECK_RIGHT" "KEYWORD_CHECK_CLEAR

#endif // !KUMIR_KEYWORDS_H
