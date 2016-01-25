#ifndef __MENU_H
#define __MENU_H

#define	MENU_ITEM_MAX	20

struct MenuItem
{
	int		State;
	char	*FileName;
};

typedef struct
{
	MenuItem[MENU_ITEM_MAX];
	void (*Update)(int button);
}Menu;


#endif