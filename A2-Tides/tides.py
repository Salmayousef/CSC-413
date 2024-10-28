import pandas as pd

# Read the CSV file into a DataFrame
df = pd.read_csv('tide_data.csv')

# Convert the 'Date' column to datetime format for proper sorting and grouping
df['Date'] = pd.to_datetime(df['Date'] + ' 2024', format='%d %b %Y')

# Calculate the highest and lowest tides
highest_tide = df['Tide Height (m)'].max()
lowest_tide = df['Tide Height (m)'].min()

# Group by Date and Moon Phase, and count the number of tides per day
tide_counts = df.groupby('Date').size().reset_index(name='Tide Count')

# Merge the tide counts back into the original DataFrame
df = df.merge(tide_counts, on='Date')

# Initialize lists for each moon phase
full_moon_tides = []
last_quarter_tides = []
new_moon_tides = []
first_quarter_tides = []

# Function to process each moon phase
def process_moon_phase(df, moon_phase):
    phase_df = df[df['Moon Phase'] == moon_phase]
    result = []
    for date, group in phase_df.groupby('Date'):
        tide_count = group['Tide Count'].iloc[0]
        result.append(tide_count)
        result.extend(group['Tide Height (m)'].tolist())
    return result

# Process each moon phase
full_moon_tides = process_moon_phase(df, 'Full Moon')
last_quarter_tides = process_moon_phase(df, 'Last Quarter')
new_moon_tides = process_moon_phase(df, 'New Moon')
first_quarter_tides = process_moon_phase(df, 'First Quarter')

# Output the results
print("Highest Tide:", highest_tide)
print("Lowest Tide:", lowest_tide)
print("Full Moon Tides:", full_moon_tides)
print("Last Quarter Tides:", last_quarter_tides)
print("New Moon Tides:", new_moon_tides)
print("First Quarter Tides:", first_quarter_tides)