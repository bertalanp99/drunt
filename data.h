////////////////////
/// ENUM SECTION ///
////////////////////

typedef enum {
    January,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December
} Month;

typedef enum {
    jan,
    feb,
    mar,
    apr,
    may,
    jun,
    jul,
    aug,
    sep,
    oct,
    nov,
    dev
} ShortMonth;

typedef enum {
    Short,
    Long
} Format;


////////////////////
// STRUCT SECTION //
////////////////////

typedef struct {
    int year; 
    int month;
    int day;
} Date;

typedef struct {
    int hour;
    int minute;
} Time;

typedef struct {
    Date date;
    Time time;
} DateTime;

typedef struct {
    char longName[10];
    char shortName[4];
} MonthName;

typedef struct {
    DateTime start;
    DateTime end;
    char name[1024];
    char location[1024];
    char description[8192];
    int priority;
} Event;

/////////////////////
/// ARRAY SECTION ///
/////////////////////

int monthLength[12] = {
    31, // January
    28, // February
    31, // March
    30, // April
    31, // May
    30, // June
    31, // July
    31, // August
    30, // September
    31, // October
    30, // November
    31  // December
};

MonthName MonthNames[12] = {
    { "January"     ,   "Jan" },
    { "February"    ,   "Feb" },
    { "March"       ,   "Mar" },
    { "April"       ,   "Apr" },
    { "May"         ,   "May" },
    { "June"        ,   "Jun" },
    { "July"        ,   "Jul" },
    { "August"      ,   "Aug" },
    { "September"   ,   "Sep" },
    { "October"     ,   "Oct" },
    { "November"    ,   "Nov" },
    { "December"    ,   "Dec" }
};
