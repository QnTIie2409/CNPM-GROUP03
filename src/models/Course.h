#ifndef COURSE_H
#define COURSE_H

#include <QString>

struct Course {
    int courseId = 0;
    QString code;
    QString name;
    int lecturerUserId = 0;
    QString lecturerName;
};

#endif
