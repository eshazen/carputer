# ChatGPT:
# please write a python script to read a CSV file and report
# statistics including: type of each column, minimum and maximum
# values for numeric columns, and minimum and maximum lengths of
# string columns.

# N.B. requires a line with column headings!

import csv
import sys

def analyze_csv(filename):
    with open(filename, newline='', encoding='utf-8') as f:
        reader = csv.DictReader(f)
        data = list(reader)

    if not data:
        print("CSV file is empty.")
        return

    stats = {}
    for field in reader.fieldnames:
        stats[field] = {
            "type": None,
            "numeric_values": [],
            "string_lengths": []
        }

    # Analyze rows
    for row in data:
        for field, value in row.items():
            if value is None or value.strip() == "":
                continue  # skip empty cells
            # Try to interpret as number
            try:
                num = float(value)
                stats[field]["numeric_values"].append(num)
            except ValueError:
                stats[field]["string_lengths"].append(len(value))

    # Summarize
    for field in reader.fieldnames:
        numeric = stats[field]["numeric_values"]
        strings = stats[field]["string_lengths"]

        if numeric and not strings:
            stats[field]["type"] = "numeric"
            stats[field]["min"] = min(numeric)
            stats[field]["max"] = max(numeric)
        elif strings and not numeric:
            stats[field]["type"] = "string"
            stats[field]["min_length"] = min(strings)
            stats[field]["max_length"] = max(strings)
        else:
            # Mixed types
            stats[field]["type"] = "mixed"
            if numeric:
                stats[field]["min_numeric"] = min(numeric)
                stats[field]["max_numeric"] = max(numeric)
            if strings:
                stats[field]["min_length"] = min(strings)
                stats[field]["max_length"] = max(strings)

    # Report
    for field in reader.fieldnames:
        print(f"Column: {field}")
        col_stats = stats[field]
        print(f"  Type: {col_stats['type']}")
        if col_stats['type'] == "numeric":
            print(f"  Min: {col_stats['min']}")
            print(f"  Max: {col_stats['max']}")
        elif col_stats['type'] == "string":
            print(f"  Min length: {col_stats['min_length']}")
            print(f"  Max length: {col_stats['max_length']}")
        else:  # mixed
            if "min_numeric" in col_stats:
                print(f"  Min numeric: {col_stats['min_numeric']}")
                print(f"  Max numeric: {col_stats['max_numeric']}")
            if "min_length" in col_stats:
                print(f"  Min length: {col_stats['min_length']}")
                print(f"  Max length: {col_stats['max_length']}")
        print()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python analyze_csv.py <filename.csv>")
    else:
        analyze_csv(sys.argv[1])
