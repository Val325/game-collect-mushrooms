int genRandoms(int lower, int upper) 
{ 
    return (rand() % (upper - lower + 1)) + lower; 
}   
