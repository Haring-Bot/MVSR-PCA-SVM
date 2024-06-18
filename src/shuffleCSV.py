import csv
import random

def shuffle_csv(input_file, output_file):
    # Read the CSV file
    with open(input_file, 'r', newline='') as csvfile:
        reader = csv.reader(csvfile)
        lines = list(reader)
    
    # Shuffle the lines (excluding the header if there is one)
    header = lines[0] if lines else []
    data_lines = lines[1:] if len(lines) > 1 else []
    random.shuffle(data_lines)
    
    # Combine header and shuffled data lines
    shuffled_lines = [header] + data_lines if header else data_lines
    
    # Write the shuffled lines to the new CSV file
    with open(output_file, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        writer.writerows(shuffled_lines)


def main():
    inputFile = "../data/emnist.csv"
    outputFile = "../data/emnistShuffled.csv"
    shuffle_csv(inputFile, outputFile)

if __name__ == "__main__":
    main()