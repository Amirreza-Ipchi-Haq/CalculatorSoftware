#include<gtk/gtk.h>
#include"cstrcal.h"
#ifdef _WIN32
#include<windows.h>
void init(){
	SetEnvironmentVariable("GSK_RENDERER","gl");
	return;
}
#else
#include<unistd.h>
void init(){
	setenv("GSK_RENDERER","gl",1);
	return;
}
#endif
GtkWidget *entry,*history;
void validateEntry(GtkWidget *widget,gpointer data){
	GtkEntryBuffer *buffer=gtk_entry_get_buffer(GTK_ENTRY(entry));
	if(!gtk_entry_buffer_get_length(buffer))
		return;
	char *str=dynastr_strnrtmp(gtk_entry_buffer_get_text(buffer),1,0),check=str[0];
	free(str);
	if((check<'0'||check>'9')&&check!='+'&&check!='-'&&check!='*'&&check!='/'&&check!='%'&&check!='.'&&check!='('&&check!=')')
		gtk_entry_buffer_delete_text(buffer,gtk_entry_buffer_get_length(buffer)-1,1);
	return;
}
void append(GtkWidget *widget,gpointer data){
	GtkEntryBuffer *buffer=gtk_entry_get_buffer(GTK_ENTRY(entry));
	gtk_entry_buffer_insert_text(buffer,gtk_entry_buffer_get_length(buffer),data,1),
	gtk_entry_grab_focus_without_selecting(GTK_ENTRY(entry)),
	gtk_editable_set_position(GTK_EDITABLE(entry),gtk_entry_buffer_get_length(buffer));
	return;
}
void clear(GtkWidget *widget,gpointer data){
	GtkEntryBuffer *buffer=gtk_entry_get_buffer(GTK_ENTRY(entry));
	gtk_entry_buffer_delete_text(buffer,0,-1),
	gtk_entry_grab_focus_without_selecting(GTK_ENTRY(entry));
	return;
}
#define OPERATION(c) (c=='+'||c=='-'||c=='*'||c=='/'||c=='%')
char *fix(char *s){
	s=dynastr_strtmp(s,1);
	size_t len=strlen(s);
	for(size_t i=0;i<len&&(s[i]=='+'||s[i]=='-');i++)
		if(s[i]=='+')
			s=dynastr_mltstr(dynastr_strappend(dynastr_strntmp(s,i,0),dynastr_strnrtmp(s,(--len)-i,0),3),s,0,3),i--;
	while(s[0]=='-'&&s[1]=='-')
		s=dynastr_strnrtmp(s,len-=2,1);
	return s;
}
char operationFound(char *s,char priority){
	for(size_t i=strlen(s);--i;)
		if((priority?s[i]=='*'||s[i]=='/'||s[i]=='%':s[i]=='+'||s[i]=='-')&&!OPERATION(s[i-1]))
			return 1;
	return 0;
}
size_t operationIndex(char *s,char priority){
	size_t len=strlen(s);
	for(size_t i=1;i<len;i++)
		if((priority?s[i]=='*'||s[i]=='/'||s[i]=='%':s[i]=='+'||s[i]=='-')&&!OPERATION(s[i-1]))
			return i;
	return 0;
}
void output(char *s){
	gtk_text_view_set_editable(GTK_TEXT_VIEW(history),1);
	GtkTextBuffer *buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(history));
	GtkTextIter start,end;
	gtk_text_buffer_get_start_iter(buffer,&start),
	gtk_text_buffer_get_end_iter(buffer,&end),
	s=dynastr_strappend(gtk_text_buffer_get_text(buffer,&start,&end,FALSE),s,0),
	gtk_text_buffer_set_text(buffer,s,-1),
	free(s),
	gtk_text_buffer_get_end_iter(buffer,&end),
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(history),gtk_text_buffer_create_mark(buffer,NULL,&end,FALSE),0,TRUE,0,1),
	gtk_text_view_set_editable(GTK_TEXT_VIEW(history),0);
	return;
}
char *solve(char *s){
	if(!s[0]){
		free(s),output("\n\nEmpty expression found!");
		return NULL;
	}
	for(size_t i=0;1;i=0){
		while((s[i]!='('||(i&&!OPERATION(s[i-1])))&&s[i])
			i++;
		if(s[i]){
			size_t j=i+1;
			unsigned long long nest=1;
			for(char recursive=0;nest&&s[j];j++){
				if(s[j]=='('&&(OPERATION(s[j-1])||s[j-1]=='(')&&!recursive)
					nest++;
				else if(s[j]=='('&&!recursive)
					recursive=1;
				if(s[j]==')'&&recursive)
					recursive=0;
				else if(s[j]==')')
					nest--;
			}
			if(nest){
				free(s),output("\n\nNot enough closing brackets!");
				return NULL;
			}
			j--;
			char *s0=dynastr_strntmp(s,i,0),*s1=dynastr_strnrtmp(s,strlen(s)-j-1,0);
			s=solve(dynastr_strnrtmp(dynastr_strntmp(s,j,1),j-i-1,1));
			if(!s){
				free(s0),free(s1);
				return NULL;
			}
			s=dynastr_strappend(dynastr_strappend(s0,s,3),s1,3);
		}
		else
			break;
	}
	for(char i=2,*s0,*s1,*s2,o;i--;)
		for(size_t j,k,l;operationFound(s,i);s=dynastr_mltstr(dynastr_strappend(dynastr_strappend(dynastr_strntmp(s,k+1,0),s2,3),dynastr_strnrtmp(s,strlen(s)-l,0),3),s,0,3)){
			j=operationIndex(s,i),k=j,l=j,o=s[j];
			while(k--&&!OPERATION(s[k]));
			while(k--&&OPERATION(s[k]));
			if(k+1)
				k++;
			while(s[++l]&&OPERATION(s[l]));
			while(s[++l]&&!OPERATION(s[l]));
			s0=fix(dynastr_strnrtmp(dynastr_strntmp(s,j,0),j-k-1,1)),s1=fix(dynastr_strnrtmp(dynastr_strntmp(s,l,0),l-j-1,1));
			if(!(strcal_isrnum(s0)&&strcal_isrnum(s1))){
				free(s),free(s0),free(s1),output("\n\nNon-number operand found!");
				return NULL;
			}
			s2=strcal_rcalculate(strcal_fixrnum(s0,1),strcal_fixrnum(s1,1),o,3);
			if(!s2){
				free(s),free(s2),output("\n\nCannot divide by zero!");
				return NULL;
			}
		}
	s=fix(s);
	if(!strcal_isrnum(s)){
		free(s),output("\n\nNon-number operand found!");
		return NULL;
	}
	return strcal_fixrnum(s,1);
}
void answer(GtkWidget *widget,gpointer data){
	GtkEntryBuffer *buffer=gtk_entry_get_buffer(GTK_ENTRY(entry));
	char *s=solve(dynastr_strtmp(gtk_entry_buffer_get_text(buffer),0));
	if(s)
		output("\n\n"),
		output(gtk_entry_buffer_get_text(buffer)),
		output("="),
		output(s),
		gtk_entry_buffer_set_text(buffer,s,-1),
		free(s);
	gtk_entry_grab_focus_without_selecting(GTK_ENTRY(entry)),
	gtk_editable_set_position(GTK_EDITABLE(entry),gtk_entry_buffer_get_length(buffer));
	return;
}
void activate(GtkApplication *app,gpointer userData){
	GtkWidget *grid=gtk_grid_new(),*window=gtk_application_window_new(app),*scroll=gtk_scrolled_window_new(),*button=gtk_button_new_with_label("0");
	gtk_window_set_title(GTK_WINDOW(window),"Calculator Software"),
	entry=gtk_entry_new(),
	g_signal_connect(GTK_EDITABLE(entry),"changed",G_CALLBACK(validateEntry),NULL);
	g_signal_connect(entry,"activate",G_CALLBACK(answer),NULL),
	history=gtk_text_view_new(),
	gtk_widget_set_can_focus(history,FALSE),
	gtk_text_view_set_editable(GTK_TEXT_VIEW(history),0),
	gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scroll),history),
	g_signal_connect(button,"clicked",G_CALLBACK(append),"0"),
#define ALIGN(widget) (gtk_widget_set_halign(widget,GTK_ALIGN_FILL),gtk_widget_set_valign(widget,GTK_ALIGN_FILL),gtk_widget_set_hexpand(widget,1),gtk_widget_set_vexpand(widget,1))
	ALIGN(grid),
	ALIGN(scroll),
	ALIGN(history),
	ALIGN(entry),
	ALIGN(button),
	gtk_window_set_child(GTK_WINDOW(window),grid),
	gtk_grid_attach(GTK_GRID(grid),scroll,0,0,5,1),
	gtk_grid_attach(GTK_GRID(grid),entry,0,1,5,1),
	gtk_grid_attach(GTK_GRID(grid),button,0,5,1,1),
#define CONFIG_BUTTON(str,str0,c,r) (button=gtk_button_new_with_label(str),g_signal_connect(button,"clicked",G_CALLBACK(append),str0),ALIGN(button),gtk_grid_attach(GTK_GRID(grid),button,c,r,1,1))
	CONFIG_BUTTON("1","1",0,4),
	CONFIG_BUTTON("2","2",1,4),
	CONFIG_BUTTON("3","3",2,4),
	CONFIG_BUTTON("4","4",0,3),
	CONFIG_BUTTON("5","5",1,3),
	CONFIG_BUTTON("6","6",2,3),
	CONFIG_BUTTON("7","7",0,2),
	CONFIG_BUTTON("8","8",1,2),
	CONFIG_BUTTON("9","9",2,2),
	CONFIG_BUTTON("+","+",3,2),
	CONFIG_BUTTON("-","-",3,3),
	CONFIG_BUTTON("*","*",3,4),
	CONFIG_BUTTON("/","/",4,3),
	CONFIG_BUTTON("mod","%",4,4),
	CONFIG_BUTTON(".",".",1,5),
	CONFIG_BUTTON("(","(",2,5),
	CONFIG_BUTTON(")",")",3,5),
#undef CONFIG_BUTTON
	button=gtk_button_new_with_label("C"),
	g_signal_connect(button,"clicked",G_CALLBACK(clear),NULL),
	ALIGN(button),
	gtk_grid_attach(GTK_GRID(grid),button,4,2,1,1),
	button=gtk_button_new_with_label("="),
	g_signal_connect(button,"clicked",G_CALLBACK(answer),NULL),
	ALIGN(button),
#undef ALIGN
	gtk_grid_attach(GTK_GRID(grid),button,4,5,1,1),
	gtk_entry_grab_focus_without_selecting(GTK_ENTRY(entry)),
	gtk_window_present(GTK_WINDOW(window));
	return;
}
int main(int argc,char **argv){
	init();
	GtkApplication *app=gtk_application_new("com.github.Amirreza-Ipchi-Haq.CalculatorSoftware",G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app,"activate",G_CALLBACK(activate),NULL);
	int status=g_application_run(G_APPLICATION(app),argc,argv);
	g_object_unref(app);
	return status;
}
