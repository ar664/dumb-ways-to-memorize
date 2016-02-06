#ifndef __MENU_H
#define __MENU_H

#define	MENU_ITEM_MAX	20

typedef struct 
{
	int		State;
	char	*FileName;
}MenuItem;

typedef struct
{
	MenuItem Items[MENU_ITEM_MAX];
	void (*Update)(int button);
}Menu;


#endif