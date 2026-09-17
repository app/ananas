#include "dselectuser.h"

#include <qvariant.h>
#include <qimage.h>
#include <qpixmap.h>

/*
 *  Constructs a dSelectUser as a child of 'parent', with the
 *  name 'name' and widget flags set to 'f'.
 *
 *  The dialog will by default be modeless, unless you set 'modal' to
 *  true to construct a modal dialog.
 */
dSelectUser::dSelectUser(QWidget* parent, const char* name, bool modal, Qt::WindowFlags fl)
    : QDialog(parent, fl)
{
    Q_UNUSED(name);
    setModal(modal);
    setupUi(this);

}

/*
 *  Destroys the object and frees any allocated resources
 */
dSelectUser::~dSelectUser()
{
    // no need to delete child widgets, Qt does it all for us
}

/*
 *  Sets the strings of the subwidgets using the current
 *  language.
 */
void dSelectUser::languageChange()
{
    retranslateUi(this);
}

void 
dSelectUser::setData( aRole *i )
{
 qulonglong id = i->sysValue("id").toULongLong();
 listBox1->clear();
 listId.clear();
 role = i;
 aUser *user = new aUser(i->db);
 user->Select();
 if(user->First())
 {
  do
  {
   if(!user->hasRole(id))
   {
    listBox1->addItem(user->sysValue("login").toString());
    listId.append(user->sysValue("id").toString());
   }
  }while(user->Next());
 }

 
}


qulonglong
dSelectUser::getData( )
{
    return userId;
}


void dSelectUser::onSelect()
{
 if(listBox1->currentRow()!=-1)
 {
  printf("add user\n");
 
  //emit( addRole(listId[listBox1->currentRow()].toULongLong()) );
  userId =  listId[listBox1->currentRow()].toULongLong();
printf("addUser()\n");
  role->addUser(userId);
  printf("user added\n");
  listBox1->takeItem(listBox1->currentRow());
  accept();
 // new aListViewItem(p_item,
 }
 else
 {
  printf("not selected\n");
 }
}
