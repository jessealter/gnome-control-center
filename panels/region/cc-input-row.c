/*
 * Copyright © 2018 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <config.h>
#include "cc-input-row.h"
#include "cc-input-source-ibus.h"

struct _CcInputRow
{
  GtkListBoxRow    parent_instance;

  CcInputSource   *source;

  GtkWidget       *name_label;
  GtkWidget       *icon_image;
};

G_DEFINE_TYPE (CcInputRow, cc_input_row, GTK_TYPE_LIST_BOX_ROW)

static void
cc_input_row_dispose (GObject *object)
{
  CcInputRow *self = CC_INPUT_ROW (object);

  g_clear_object (&self->source);

  G_OBJECT_CLASS (cc_input_row_parent_class)->dispose (object);
}

void
cc_input_row_class_init (CcInputRowClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  object_class->dispose = cc_input_row_dispose;

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/control-center/region/cc-input-row.ui");
  gtk_widget_class_bind_template_child (widget_class, CcInputRow, name_label);
  gtk_widget_class_bind_template_child (widget_class, CcInputRow, icon_image);
}

void
cc_input_row_init (CcInputRow *row)
{
  gtk_widget_init_template (GTK_WIDGET (row));
}

static void
label_changed_cb (CcInputRow *row)
{
  g_autofree gchar *label = cc_input_source_get_label (row->source);
  gtk_label_set_text (GTK_LABEL (row->name_label), label);
}

CcInputRow *
cc_input_row_new (CcInputSource *source)
{
  CcInputRow *row;

  row = g_object_new (CC_TYPE_INPUT_ROW, NULL);
  row->source = g_object_ref (source);

  g_signal_connect_object (source, "label-changed", G_CALLBACK (label_changed_cb), row, G_CONNECT_SWAPPED);
  label_changed_cb (row);

  gtk_widget_set_visible (row->icon_image, CC_IS_INPUT_SOURCE_IBUS (source));

  return row;
}

CcInputSource *
cc_input_row_get_source (CcInputRow *row)
{
  g_return_val_if_fail (CC_IS_INPUT_ROW (row), NULL);
  return row->source;
}
