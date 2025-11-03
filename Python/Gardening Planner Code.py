# 🌿🌱 Gardening Planner 🌱🌿
from math import modf
print("☘️🌱🪴 Welcome to the Ultimate Gardening Planner 🌿🌴🌳")
print("====================================================\n")
# 🌳 Plant details
n = input("🌳 Plant Name 🌴: ")
n1 = input("🍉🍑🍏 Fruits 🥥🍌🍇: ")
n2 = input("🌺🌹 Flowers 🌻🌸🏵️: ")
n3 = input("🌷 Aquatic 🪻: ")
n4 = input("🍈🥦🥒 Vegetables 🥕🫛🫘: ")
n5 = input("🥜 Medicinal 🫚: ")

# 🪴 Pot details
n6 = input("🪴 Pot and Location 🪴: ")
base = float(input("Base of Pot (cm): "))
height = float(input("Height of Pot (cm): "))
area_of_pot = 0.5 * base * height  # triangular pot

# 🌅 Sunlight and equipment
n7 = input("🌅 Sunlight Requirement 🌦️: ")
n8 = input("🎋 Equipment ⛏️🔨: ")

# ⚗️ Fertilizer input (space separated numbers)
n9 = input("⚗️ Fertilizers 🧪 (space separated numbers): ")
fertilizers = list(map(int, n9.split()))

# Fractional part of pot area (optional)
frac_part, int_part = modf(area_of_pot)

# 🪴 Display all details neatly
print("\n☘️🌱🪴================= Gardening Planner =================🌿🌴🌳\n")
print(f"🌳 Plant Name: {n}")
print(f"🍉 Fruits: {n1}")
print(f"🌺 Flowers: {n2}")
print(f"🌷 Aquatic: {n3}")
print(f"🍈 Vegetables: {n4}")
print(f"🥜 Medicinal: {n5}\n")

print(f"🪴 Pot & Location: {n6}")
print(f"📏 Pot Area: {area_of_pot} cm² (Fractional part: {frac_part:.2f})\n")

print(f"🌅 Sunlight Requirement: {n7}")
print(f"🎋 Equipment: {n8}\n")

print(f"⚗️ Fertilizers List: {fertilizers}")
print(f"🔝 Maximum Fertilizer: {max(fertilizers)}")
print(f"🔽 Minimum Fertilizer: {min(fertilizers)}\n")

print("☘️🌱🪴==================== Happy Gardening ===================🌿🌴🌳")
print("❤️===❤️=== Happy Life ===❤️===❤️")
