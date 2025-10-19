.mode csv
.output airport.csv
select 2,City,LocationID,ARPLatitudeS/3600.0, ARPLongitudeS/3600.0 from airport 
       where Type='AIRPORT' 
       and State <> 'AS' 
       and State <> 'GU' 
       and State <> 'HI' 
       and State <> 'MP' 
       and State <> 'AK' 
       and State <> 'PR' 
       and State <> 'TT' 
       and State <> 'VI' order by State,City;
