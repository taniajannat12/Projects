name = input("Enter name: ")
student_id = int(input("Enter ID: "))
password = input("Enter password: ")
department = input("Enter Department: ")
course = input("Enter course name: ")

print("====================================")
print("🎓 STUDENTS GRADING SYSTEM 🎓")
print("👩‍🏫 Student Name:", name)
print("Student ID:", student_id)
academic_year = input("Enter Academic Year (e.g., 2025): ")
print("Academic Year:", academic_year)
if len(password) >= 5 and any(char.isdigit() for char in password):
   print("right🎊")
else:
   print("Wrong! Try again😊")
print("Student Department:", department)
print("Student Course:", course)

marks = int(input("Enter marks: "))
print("====================================")

if marks >= 80:
    grade = "A+"
    remark = "Excellent🎇"
elif marks >= 70:
    grade = "A"
    remark = "Very Good🎉"
elif marks >= 60:
    grade = "A-"
    remark = "Good😊"
elif marks >= 50:
    grade = "B"
    remark = "Needs Improvement🥲"
else:
    grade = "Fail😢🥲"
    remark = "Better🤲 Luck Next Time"

print("Students Grade:", grade)
print("Teachers Remarks:", remark)

if marks >= 50:
    print("Congratulations🎉🎉🎉 You passed")
else:
    print("🥺🥺🥹 Sorry, You failed. Better luck next time")

print("All the best for Your Future ❤❤❤")