
int cmd_rmdir(char dir[])
{

char		temp[256];
kol_struct70	k70;
unsigned	result;

if (NULL == dir)
	{
	printf("  rmdir directory\n\r");
	return FALSE;
	}

if ( ( 0 == strcmp(dir, ".") ) || ( 0 == strcmp(dir, "..") ) || ( 0 == strcmp(cur_dir, "/")) ) 
	{
	return FALSE;
	}

k70.p00 = 8;
k70.p04 = 0;
k70.p08 = 0;
k70.p12 = 0;
k70.p16 = 0;
k70.p20 = 0;

if ( '/' == dir[0])
	k70.p21 = dir;
else
	{
	strcpy(temp, cur_dir);
	strcat(temp, dir);
	k70.p21 = temp;
	}

if ( !dir_check(temp) )
	return FALSE;

result = kol_file_70(&k70);

if (0 == result)
	return TRUE;
else 
	return FALSE;

}
