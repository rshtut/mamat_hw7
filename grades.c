#include "linked-list.h"
#include "grades.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

void* find_student(struct list *students, int id);

// this is a struct to hold a course and it's grade
struct course {
	char *name;
	int grade;
};

// this is a struct that holds a student and a list of his courses
struct student {
	struct list *courses;
	char *name;
	int id;
};


// this is the main struct. it hold a list of students.
struct grades {
	struct list *students;
};

// this is the clone function for courses.
int clone_course(void *this_course, void **output_course) {
	struct course *new_course, *cur_course;
	cur_course=(struct course*)this_course;
	new_course=(struct course*)malloc(sizeof(struct course*));
	if (!new_course) {
		return -1;
	}
	new_course->name = (char*)malloc(strlen(cur_course->name));
	strcpy(new_course->name,cur_course->name);
	new_course->grade=cur_course->grade;
	*output_course=new_course;
	return 0;
}

//destroy fuction for courses
void course_destroy(void *course) {
	free(course);
}

//clone function for students
int clone_student(void *this_student, void **output_student) {
	struct student *new_student, *cur_student;
	cur_student=(struct student*)this_student;
	new_student=(struct student*)malloc(sizeof(struct student*));
	if (!new_student) {
		return -1;
	}
	new_student->name = (char*)malloc(strlen(cur_student->name));
	strcpy(new_student->name,cur_student->name);
	new_student->id=cur_student->id;
	struct node *cur_course, *tmp_course;
	//initializing a course list for the new student
	cur_course=list_begin(cur_student->courses);
	new_student->courses=list_init(clone_course, course_destroy);
	//cloning the courses from the original student
	while(cur_course) {
		tmp_course=cur_course;
		list_push_back(new_student->courses, tmp_course);
		cur_course=list_next(tmp_course);
	}
	*output_student=new_student;
	return 0;
}

//destroy function for student
void student_destroy(void *student) {
	struct student *cur_student;
	cur_student=(struct student*)student;
	//calling the destroy function for the course list
	list_destroy(cur_student->courses);
	free(student);
}


//initializing the grades struct with zero students
struct grades* grades_init() {
	struct grades *grades;
	grades = (struct grades*)malloc(sizeof(struct grades*));
    if (!grades) {
        return NULL;
    }
    grades->students=list_init(clone_student, student_destroy);
    return grades;
}

//destroyind the grades
void grades_destroy(struct grades *grades) {
	//destroying the students list using the destroy student function
	list_destroy(grades->students);
	free(grades);
}

//adding a new student to grades
int grades_add_student(struct grades *grades, const char *name, int id) {
	if (grades == NULL) {
		return -1;
	}
	// making sure the student doesn't already exist
	if (find_student(grades->students, id) != NULL) {
		return -1;
	}
	struct student *new_student;
	new_student=(struct student*)malloc(sizeof(struct student*));
	if (!new_student) {
		return -1;
	}
	new_student->id=id;
	new_student->name = (char *)malloc(strlen(name));
	strcpy(new_student->name,name);
	new_student->courses=list_init(clone_course,course_destroy);
	list_push_back(grades->students,new_student);
	return 0;
}

//adding a grade to a student
int grades_add_grade(struct grades *grades,
                     const char *name,
                     int id,
                     int grade)
{
	if (grades==NULL) {
		return -1;
	}
	//checking that the grade is valid
	if (grade<0 || grade>100) {
		return -1;
	}
	struct student *student = find_student(grades->students, id);
	//checking that the student exists
	if(!student) {
		return -1;
	}
	struct node *cur_course_node;
	struct course *cur_course;
	cur_course_node=list_begin(student->courses);
	while (cur_course_node) {
		cur_course=list_get(cur_course_node);
		//returning if the course already exists
		if (strcmp(cur_course->name,name)==0) {
			return -1;
		}
		cur_course_node=list_next(cur_course_node);
	}
	struct course *course;
	course=(struct course*)malloc(sizeof(struct course*));
	if (!course) {
		return -1;
	}
	course->name = (char*)malloc(strlen(name));
	strcpy(course->name,name);
	course->grade=grade;
	list_push_back(student->courses,course);
	return 0;
}

//calculating average
float grades_calc_avg(struct grades *grades, int id, char **out) {

	if (grades==NULL) {
		out=NULL;
		return -1;
	}
	
	struct student *student = find_student(grades->students, id);
	//checking if the student exists
	if(!student) {
		out=NULL;
		return -1;
	}

	*out=malloc(sizeof(char)*(strlen(student->name)));
	strcpy(*out,student->name);
	float average=0, tot_grades=0;
	int counter=0;
	struct node *cur_course;
	struct course *course;
	cur_course=list_begin(student->courses);
	//if there's no courses return 0
	if (cur_course==NULL) {
		return 0;
	}
	while (cur_course) {
		course=list_get(cur_course);
		tot_grades+=(float)course->grade;
		counter++;
		cur_course=list_next(cur_course);
	}
	average=tot_grades/counter;
	return average;
}

//printing student data
int grades_print_student(struct grades *grades, int id) {
	if (grades==NULL) {
		return -1;
	}
	
	struct student *student = find_student(grades->students, id);
	//checking that student exists
	if(!student) {
		return -1;
	}

	printf("%s %d:", student->name, student->id);
	struct node *cur_course;
	struct course *course;
	cur_course=list_begin(student->courses);
	bool first = 0;
	while (cur_course) {
		course=list_get(cur_course);
		if (!first) {
			printf(" %s %d", course->name, course->grade);
			first = 1;
		}
		else {
			printf(", %s %d", course->name, course->grade);
		}
		cur_course=list_next(cur_course);
	}
	printf("\n");
	return 0;
}

//printing all stidents data
int grades_print_all(struct grades *grades) {
	if (grades==NULL) {
		return -1;
	}
	struct node *cur_student;
	struct student *student;
	cur_student=list_begin(grades->students);
	while(cur_student) {
		student=list_get(cur_student);
		grades_print_student(grades,student->id);
		cur_student=list_next(cur_student);
	}
	return 0;
}

//finds a stundent with id in grades. returns NULL if doesn't exist
void* find_student(struct list *students, int id) {
	struct node *student_node;
	struct student *student;
	student_node=list_begin(students);
	while(student_node) {
		student=list_get(student_node);
		if (student->id == id) {
			return student;
		}
		student_node=list_next(student_node);
	}
	return NULL;
}